use crate::ast::Expression;
use crate::object::{ObjData, ObjId, ProcKind, SruProc};
use crate::symbol;
use crate::symbol::SymbolId;
use crate::vm::Vm;

#[derive(Debug, Clone)]
pub struct StackFrame {
    pub expressions: Vec<Expression>,
    pub tree_it: usize,
    pub operations: Vec<TraceOp>,
    pub it: usize,
    pub local_stack: Vec<ObjId>,
    pub binding: ObjId,
    pub upper: Option<Box<StackFrame>>,
    /// Name of the method or function this frame is executing, for
    /// stack-trace display. `None` for the top-level frame.
    pub name: Option<SymbolId>,
}

impl Drop for StackFrame {
    fn drop(&mut self) {
        // Iteratively drop the `upper` chain. Otherwise a deep chain (e.g.,
        // a tight while-loop's recursive desugaring) overflows the Rust
        // stack when the outermost frame drops.
        let mut cur = self.upper.take();
        while let Some(mut boxed) = cur {
            cur = boxed.upper.take();
        }
    }
}

impl StackFrame {
    pub fn new(binding: ObjId, expressions: Vec<Expression>) -> Self {
        Self {
            expressions,
            tree_it: 0,
            operations: Vec::new(),
            it: 0,
            local_stack: Vec::new(),
            binding,
            upper: None,
            name: None,
        }
    }
}

#[derive(Debug, Clone)]
pub struct TraceOp {
    pub kind: TraceKind,
    pub pos: crate::ast::Pos,
}

#[derive(Debug, Clone)]
pub enum TraceKind {
    PushStr(String),
    LookupRef { var: SymbolId, has_env: bool },
    Assign { var: SymbolId, has_env: bool },
    Call { method: SymbolId, argc: usize, has_recv: bool },
    MakeProc {
        vargs: Vec<SymbolId>,
        retval: Option<SymbolId>,
        body: Vec<Expression>,
    },
    Pop,
}

impl TraceOp {
    pub fn new(kind: TraceKind, pos: crate::ast::Pos) -> Self {
        Self { kind, pos }
    }
}

/// Flatten an AST node into TraceOps, post-order: children, then the node.
pub fn trace(expr: &Expression, out: &mut Vec<TraceOp>) {
    let pos = expr.pos();
    match expr {
        Expression::StrLit { value, .. } => {
            out.push(TraceOp::new(TraceKind::PushStr(value.clone()), pos))
        }
        Expression::Ref { var, env, .. } => {
            if let Some(e) = env {
                trace(e, out);
                out.push(TraceOp::new(
                    TraceKind::LookupRef { var: *var, has_env: true },
                    pos,
                ));
            } else {
                out.push(TraceOp::new(
                    TraceKind::LookupRef { var: *var, has_env: false },
                    pos,
                ));
            }
        }
        Expression::Let { var, env, value, .. } => {
            if let Some(e) = env {
                trace(e, out);
            }
            trace(value, out);
            out.push(TraceOp::new(
                TraceKind::Assign {
                    var: *var,
                    has_env: env.is_some(),
                },
                pos,
            ));
        }
        Expression::Call { receiver, method, args, .. } => {
            if let Some(r) = receiver {
                trace(r, out);
            }
            for a in args {
                trace(a, out);
            }
            out.push(TraceOp::new(
                TraceKind::Call {
                    method: *method,
                    argc: args.len(),
                    has_recv: receiver.is_some(),
                },
                pos,
            ));
        }
        Expression::Proc { vargs, retval, body, .. } => {
            out.push(TraceOp::new(
                TraceKind::MakeProc {
                    vargs: vargs.clone(),
                    retval: *retval,
                    body: body.clone(),
                },
                pos,
            ));
        }
    }
}

pub enum StepResult {
    Continue,
    PushFrame(StackFrame),
    EndOfFrame,
}

/// Evaluate one step. The active StackFrame lives in vm.current_frame.
pub fn step(vm: &mut Vm) -> StepResult {
    // Safe point for GC: between ops, all live ObjIds are reachable from
    // the current frame's upper chain, bindings, and builtin refs. No
    // Native function has Rust-local ObjIds at this point.
    if vm.heap.should_gc() {
        crate::gc::collect(vm);
    }

    // Are there ops to run in the current operations list?
    let needs_next_stmt = {
        let f = vm.frame();
        f.it >= f.operations.len()
    };

    if needs_next_stmt {
        // Move to the next statement, or end frame.
        let (has_next, idx, is_last) = {
            let f = vm.frame();
            let has = f.tree_it < f.expressions.len();
            (
                has,
                if has { f.tree_it } else { 0 },
                f.tree_it + 1 == f.expressions.len(),
            )
        };
        if !has_next {
            return StepResult::EndOfFrame;
        }
        let next_expr = {
            let f = vm.frame_mut();
            f.tree_it += 1;
            f.expressions[idx].clone()
        };
        let mut ops = Vec::new();
        trace(&next_expr, &mut ops);
        if !is_last {
            ops.push(TraceOp::new(TraceKind::Pop, crate::ast::POS_UNKNOWN));
        }
        let f = vm.frame_mut();
        f.operations = ops;
        f.it = 0;
        return StepResult::Continue;
    }

    // Execute one op.
    let op = {
        let f = vm.frame_mut();
        let op = f.operations[f.it].clone();
        f.it += 1;
        op
    };
    vm.current_pos = op.pos;

    match op.kind {
        TraceKind::PushStr(s) => {
            let id = vm.heap.alloc_with_data(ObjData::Str(s));
            let str_cls = vm.builtin.string_cls;
            vm.heap.set_slot(id, symbol::intern("klass"), str_cls);
            vm.frame_mut().local_stack.push(id);
            StepResult::Continue
        }
        TraceKind::LookupRef { var, has_env } => {
            let env = if has_env {
                vm.frame_mut().local_stack.pop().unwrap()
            } else {
                vm.frame().binding
            };
            let val = lookup(vm, env, var);
            vm.frame_mut().local_stack.push(val);
            StepResult::Continue
        }
        TraceKind::Assign { var, has_env } => {
            let value = vm.frame_mut().local_stack.pop().unwrap();
            let env = if has_env {
                vm.frame_mut().local_stack.pop().unwrap()
            } else {
                vm.frame().binding
            };
            assign(vm, env, var, value);
            vm.frame_mut().local_stack.push(value);
            StepResult::Continue
        }
        TraceKind::Call { method, argc, has_recv } => do_call(vm, method, argc, has_recv),
        TraceKind::MakeProc { vargs, retval, body } => {
            let bind = vm.frame().binding;
            let proc = ProcKind::Sru(SruProc {
                vargs,
                retval,
                body,
                def_binding: bind,
            });
            let id = vm.heap.alloc_with_data(ObjData::Proc(proc));
            let proc_cls = vm.builtin.proc_cls;
            vm.heap.set_slot(id, symbol::intern("klass"), proc_cls);
            vm.frame_mut().local_stack.push(id);
            StepResult::Continue
        }
        TraceKind::Pop => {
            vm.frame_mut().local_stack.pop();
            StepResult::Continue
        }
    }
}

/// Look up `var` starting at `env`, walking the `_parent` chain on Bindings
/// and falling through to `klass.findInstanceMethod` for non-Binding envs.
/// Returns nil if not found.
pub fn lookup(vm: &mut Vm, env: ObjId, var: SymbolId) -> ObjId {
    let mut cur = env;
    loop {
        if let Some(v) = vm.heap.get_slot(cur, var) {
            return v;
        }
        // Try parent chain (Binding-style).
        let parent_sym = symbol::intern("_parent");
        match vm.heap.get_slot(cur, parent_sym) {
            Some(p) if p != vm.builtin.nil_id => {
                cur = p;
                continue;
            }
            _ => {}
        }
        // Try klass.instanceMethods chain.
        let klass_sym = symbol::intern("klass");
        let inst_sym = symbol::intern("instanceMethods");
        let super_sym = symbol::intern("superclass");
        if let Some(klass) = vm.heap.get_slot(env, klass_sym) {
            let mut k = klass;
            while k != vm.builtin.nil_id {
                if let Some(im) = vm.heap.get_slot(k, inst_sym) {
                    if let Some(v) = vm.heap.get_slot(im, var) {
                        return v;
                    }
                }
                match vm.heap.get_slot(k, super_sym) {
                    Some(s) => k = s,
                    None => break,
                }
            }
        }
        return vm.builtin.nil_id;
    }
}

/// Assign `value` to `var` on `env`. If `env` is a Binding and the parent chain
/// already has the slot, write there. Otherwise create the slot on `env`.
pub fn assign(vm: &mut Vm, env: ObjId, var: SymbolId, value: ObjId) {
    let parent_sym = symbol::intern("_parent");
    let mut cur = env;
    loop {
        if vm.heap.has_slot(cur, var) {
            vm.heap.set_slot(cur, var, value);
            return;
        }
        match vm.heap.get_slot(cur, parent_sym) {
            Some(p) if p != vm.builtin.nil_id => cur = p,
            _ => break,
        }
    }
    vm.heap.set_slot(env, var, value);
}

fn do_call(vm: &mut Vm, method: SymbolId, argc: usize, has_recv: bool) -> StepResult {
    // Pop args (and recv) from local stack.
    let stack_len = vm.frame().local_stack.len();
    let args_start = stack_len - argc;
    let args: Vec<ObjId> = vm.frame_mut().local_stack.drain(args_start..).collect();
    let recv = if has_recv {
        vm.frame_mut().local_stack.pop().unwrap()
    } else {
        vm.frame().binding
    };

    // Resolve method.
    let proc_id = if has_recv {
        let direct = vm.heap.get_slot(recv, method);
        match direct {
            Some(v) => v,
            None => lookup_method(vm, recv, method),
        }
    } else {
        lookup(vm, recv, method)
    };

    // Not-found: print a useful error and abort.
    if proc_id == vm.builtin.nil_id {
        report_call_error(vm, method, recv, has_recv);
    }

    invoke_named(vm, proc_id, recv, args, has_recv, Some(method))
}

fn report_call_error(vm: &Vm, method: SymbolId, recv: ObjId, has_recv: bool) -> ! {
    let mname = symbol::name(method);
    let msg = if has_recv {
        format!(
            "undefined method `{}` for {}",
            mname,
            crate::builtin::io::inspect(vm, recv)
        )
    } else {
        format!("undefined function `{}`", mname)
    };
    runtime_error(vm, msg)
}

/// Abort with a runtime error message and a stack trace. If the current
/// op has a known source position, render `<source line>` with a caret.
pub fn runtime_error(vm: &Vm, msg: impl AsRef<str>) -> ! {
    let pos = vm.current_pos;
    let where_ = source_loc(vm, pos);
    eprintln!("Error{}: {}", where_, msg.as_ref());
    if pos != crate::ast::POS_UNKNOWN && !vm.source.is_empty() {
        if let Some(snippet) = format_source_snippet(&vm.source, pos) {
            eprintln!("{}", snippet);
        }
    }
    print_stack_trace(vm);
    std::process::exit(1)
}

fn source_loc(vm: &Vm, pos: crate::ast::Pos) -> String {
    if pos == crate::ast::POS_UNKNOWN || vm.source.is_empty() {
        return String::new();
    }
    let (line, col, _) = locate_pos(&vm.source, pos);
    format!(" at line {} col {}", line, col)
}

fn locate_pos(src: &str, pos: usize) -> (usize, usize, String) {
    let pos = pos.min(src.len());
    let mut line = 1usize;
    let mut line_start = 0usize;
    for (i, c) in src.char_indices() {
        if i >= pos { break; }
        if c == '\n' {
            line += 1;
            line_start = i + 1;
        }
    }
    let line_end = src[line_start..]
        .find('\n')
        .map(|n| line_start + n)
        .unwrap_or(src.len());
    let line_text = src[line_start..line_end].to_string();
    let col = src[line_start..pos].chars().count() + 1;
    (line, col, line_text)
}

fn format_source_snippet(src: &str, pos: usize) -> Option<String> {
    let (line, col, line_text) = locate_pos(src, pos);
    let line_text = line_text.trim_end_matches('\n');
    let header = format!("  {} | {}", line, line_text);
    let pad = format!("  {} | ", line).chars().count() + col.saturating_sub(1);
    let caret = format!("{}^", " ".repeat(pad));
    Some(format!("{}\n{}", header, caret))
}

/// Walk the active frame's `upper` chain and print each frame's name to
/// stderr. The topmost frame (the one that hit the error) is shown first.
/// Adjacent unnamed `<top-level>` frames are collapsed into one entry so
/// the bootstrap / eval_program plumbing doesn't show up as duplicates.
pub fn print_stack_trace(vm: &Vm) {
    eprintln!("Stack trace (most recent call first):");
    let mut cur = Some(vm.frame());
    let mut depth = 0;
    let mut last_was_anon = false;
    while let Some(f) = cur {
        match f.name {
            Some(s) => {
                eprintln!("  #{} `{}`", depth, symbol::name(s));
                depth += 1;
                last_was_anon = false;
            }
            None => {
                if !last_was_anon {
                    eprintln!("  #{} <top-level>", depth);
                    depth += 1;
                    last_was_anon = true;
                }
            }
        }
        cur = f.upper.as_deref();
    }
}

fn lookup_method(vm: &mut Vm, recv: ObjId, method: SymbolId) -> ObjId {
    let klass_sym = symbol::intern("klass");
    let inst_sym = symbol::intern("instanceMethods");
    let super_sym = symbol::intern("superclass");
    if let Some(klass) = vm.heap.get_slot(recv, klass_sym) {
        let mut k = klass;
        while k != vm.builtin.nil_id {
            if let Some(im) = vm.heap.get_slot(k, inst_sym) {
                if let Some(v) = vm.heap.get_slot(im, method) {
                    return v;
                }
            }
            match vm.heap.get_slot(k, super_sym) {
                Some(s) => k = s,
                None => break,
            }
        }
    }
    vm.builtin.nil_id
}

pub fn invoke(vm: &mut Vm, proc_id: ObjId, recv: ObjId, args: Vec<ObjId>, has_recv: bool) -> StepResult {
    invoke_named(vm, proc_id, recv, args, has_recv, None)
}

pub fn invoke_named(
    vm: &mut Vm,
    proc_id: ObjId,
    recv: ObjId,
    args: Vec<ObjId>,
    has_recv: bool,
    name: Option<SymbolId>,
) -> StepResult {
    // For instance-method calls (has_recv=true), the SRU proc's first formal
    // arg is `self`, so prepend the receiver to args before binding.
    let mut bind_args = args;
    if has_recv {
        bind_args.insert(0, recv);
    }
    // Dispatch on ProcKind.
    let kind_kind = {
        let bo = vm.heap.get(proc_id);
        match &bo.data {
            Some(ObjData::Proc(k)) => Some(match k {
                ProcKind::Native(_) => 0,
                ProcKind::NativeSmash(_) => 1,
                ProcKind::Sru(_) => 2,
                ProcKind::Continuation(_) => 3,
            }),
            _ => None,
        }
    };
    match kind_kind {
        Some(0) => {
            // Native: copy fn pointer out, then call.
            let f = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::Native(f))) = &bo.data {
                    *f
                } else {
                    unreachable!()
                }
            };
            // Native convention: args[0] = receiver, args[1..] = passed args.
            let mut full = Vec::with_capacity(bind_args.len() + 1);
            if !has_recv {
                full.push(recv);
            }
            full.extend(bind_args);
            let r = f(vm, &full);
            vm.frame_mut().local_stack.push(r);
            StepResult::Continue
        }
        Some(1) => {
            let f = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::NativeSmash(f))) = &bo.data {
                    *f
                } else {
                    unreachable!()
                }
            };
            let mut full = Vec::with_capacity(bind_args.len() + 1);
            if !has_recv {
                full.push(recv);
            }
            full.extend(bind_args);
            f(vm, &full);
            StepResult::Continue
        }
        Some(2) => {
            // SRU proc. Make a new binding parented at def_binding, bind vargs, push frame.
            let (vargs, retval, body, def_bind) = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::Sru(p))) = &bo.data {
                    (p.vargs.clone(), p.retval, p.body.clone(), p.def_binding)
                } else {
                    unreachable!()
                }
            };
            let new_bind = vm.make_binding(def_bind);
            // Bind each varg. (bind_args already includes recv when has_recv.)
            for (i, name) in vargs.iter().enumerate() {
                let val = bind_args.get(i).copied().unwrap_or(vm.builtin.nil_id);
                vm.heap.set_slot(new_bind, *name, val);
            }
            if let Some(rname) = retval {
                let cont = make_continuation(vm);
                vm.heap.set_slot(new_bind, rname, cont);
            }
            let mut new_frame = StackFrame::new(new_bind, body);
            new_frame.name = name;
            StepResult::PushFrame(new_frame)
        }
        Some(3) => {
            // Continuation: restore the saved frame, push arg as the return value.
            let saved_frame_obj = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::Continuation(f))) = &bo.data {
                    *f
                } else {
                    unreachable!()
                }
            };
            let saved_frame = {
                let bo = vm.heap.get(saved_frame_obj);
                if let Some(ObjData::Frame(f)) = &bo.data {
                    f.clone()
                } else {
                    panic!("continuation target is not a frame");
                }
            };
            let ret = bind_args.into_iter().next().unwrap_or(vm.builtin.nil_id);
            *vm.frame_mut() = saved_frame;
            vm.frame_mut().local_stack.push(ret);
            StepResult::Continue
        }
        _ => {
            let nil_id = vm.builtin.nil_id;
            vm.frame_mut().local_stack.push(nil_id);
            StepResult::Continue
        }
    }
}

fn make_continuation(vm: &mut Vm) -> ObjId {
    // Save a clone of the current frame (one node only, upper chain preserved
    // by Box reuse — but Clone deep-copies via Box<StackFrame>; that's fine
    // for now and matches csru semantics where each frame holds enough state
    // to resume on its own).
    let frame_clone = vm.frame().clone();
    let frame_obj = vm.heap.alloc_with_data(ObjData::Frame(frame_clone));
    let cont = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::Continuation(frame_obj)));
    cont
}

pub fn frame_depth_of(f: &StackFrame) -> usize {
    frame_depth(f)
}

fn frame_depth(f: &StackFrame) -> usize {
    let mut d = 0;
    let mut cur = f;
    while let Some(up) = &cur.upper {
        d += 1;
        cur = up;
    }
    d
}

/// Run until we pop back to a frame at depth strictly less than `floor`.
/// `floor` is the depth that should remain active after the call completes.
fn run_until_depth(vm: &mut Vm, floor: usize) -> ObjId {
    loop {
        match step(vm) {
            StepResult::Continue => {}
            StepResult::PushFrame(mut new_frame) => {
                let is_tail = {
                    let f = vm.frame();
                    f.it >= f.operations.len() && f.tree_it >= f.expressions.len()
                };
                if is_tail {
                    // Tail call: replace the current frame in place. Keep its
                    // upper chain intact so the eventual EndOfFrame still pops
                    // back to the right caller. This is what stops a tight
                    // while-loop from accumulating an O(N) Box<StackFrame>
                    // chain.
                    let prev_upper = vm.frame_mut().upper.take();
                    new_frame.upper = prev_upper;
                    let old = std::mem::replace(vm.frame_mut(), new_frame);
                    drop(old);
                } else {
                    let prev = std::mem::replace(
                        vm.frame_mut(),
                        StackFrame::new(new_frame.binding, vec![]),
                    );
                    new_frame.upper = Some(Box::new(prev));
                    *vm.frame_mut() = new_frame;
                }
            }
            StepResult::EndOfFrame => {
                let nil_id = vm.builtin.nil_id;
                let ret = vm.frame_mut().local_stack.pop().unwrap_or(nil_id);
                let upper = vm.frame_mut().upper.take();
                match upper {
                    Some(up) => {
                        *vm.frame_mut() = *up;
                        vm.frame_mut().local_stack.push(ret);
                        if frame_depth(vm.frame()) < floor {
                            let r = vm.frame_mut().local_stack.pop().unwrap_or(nil_id);
                            return r;
                        }
                    }
                    None => return ret,
                }
            }
        }
    }
}

/// Run until the top-level frame completes.
pub fn run(vm: &mut Vm) -> ObjId {
    run_until_depth(vm, 0)
}

/// Run until we return back to the current frame (used by invoke_inline).
pub fn run_inline(vm: &mut Vm, floor: usize) -> ObjId {
    run_until_depth(vm, floor)
}

/// Convenience: evaluate a program (sequence of top-level statements) in the
/// current root frame's binding, return the last value.
pub fn eval_program(vm: &mut Vm, exprs: Vec<Expression>) -> ObjId {
    let bind = vm.frame().binding;
    let frame = StackFrame::new(bind, exprs);
    let prev = std::mem::replace(vm.frame_mut(), frame);
    // Stash the previous frame as upper so we restore on EndOfFrame.
    vm.frame_mut().upper = Some(Box::new(prev));
    run(vm)
}

