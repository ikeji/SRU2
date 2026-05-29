use crate::eval::StackFrame;
use crate::heap::Heap;
use crate::object::ObjId;
use crate::symbol;

pub struct Vm {
    pub heap: Heap,
    pub current_frame: StackFrame,
    pub root_binding: ObjId,
    pub builtin: BuiltinRefs,
    /// Registered source texts indexed by `SourceId`. Each StackFrame and
    /// SruProc remembers which entry its byte positions refer to.
    pub sources: Vec<Source>,
    /// Byte offset of the most recently dispatched TraceOp. `POS_UNKNOWN` if
    /// no real source position is known (e.g., synthesised ops).
    pub current_pos: crate::ast::Pos,
    /// The proc object currently being invoked. Native functions can read
    /// this to recover their own ObjId (useful for FFI trampolines that
    /// need a per-proc hidden slot). NO_SOURCE-like sentinel: `nil_id`
    /// when no Native is active.
    pub current_native_proc: ObjId,
}

pub type SourceId = u32;

/// Sentinel for "no source registered". Frames that hit POS_UNKNOWN
/// positions render without a snippet.
pub const NO_SOURCE: SourceId = u32::MAX;

#[derive(Debug, Clone)]
pub struct Source {
    pub name: String,
    pub content: String,
}

/// Well-known object ids set up at boot.
#[derive(Default)]
pub struct BuiltinRefs {
    pub nil_id: ObjId,
    pub true_id: ObjId,
    pub false_id: ObjId,
    pub class_cls: ObjId,
    pub object_cls: ObjId,
    pub nil_cls: ObjId,
    pub boolean_cls: ObjId,
    pub numeric_cls: ObjId,
    pub string_cls: ObjId,
    pub array_cls: ObjId,
    pub proc_cls: ObjId,
    pub binding_cls: ObjId,
    pub math_cls: ObjId,
    pub sys_cls: ObjId,
    pub hash_cls: ObjId,
    /// The `__parser` object exposed in the root binding.
    pub parser_id: ObjId,
}

const PRELUDE: &str = include_str!("prelude.sru");

fn load_prelude(vm: &mut Vm) {
    vm.eval_source_named("<prelude>", PRELUDE);
    // Reset current frame source so the script that runs next picks up
    // its own source id (assigned by run_source_named / run_bytecode).
    vm.current_frame.source = NO_SOURCE;
}

impl Vm {
    pub fn new() -> Self {
        let mut heap = Heap::new();
        let placeholder = heap.alloc_empty();
        let root_binding = placeholder;
        let mut vm = Self {
            heap,
            current_frame: StackFrame::new(root_binding, Vec::new()),
            root_binding,
            builtin: BuiltinRefs::default(),
            sources: Vec::new(),
            current_pos: crate::ast::POS_UNKNOWN,
            current_native_proc: ObjId(0),
        };
        crate::builtin::bootstrap(&mut vm);
        load_prelude(&mut vm);
        vm
    }

    #[inline]
    pub fn frame(&self) -> &StackFrame {
        &self.current_frame
    }

    #[inline]
    pub fn frame_mut(&mut self) -> &mut StackFrame {
        &mut self.current_frame
    }

    /// Register a source and return its id. The id is used by StackFrame /
    /// SruProc to locate the source content for error rendering.
    pub fn add_source(&mut self, name: impl Into<String>, content: impl Into<String>) -> SourceId {
        let id = self.sources.len() as SourceId;
        self.sources.push(Source {
            name: name.into(),
            content: content.into(),
        });
        id
    }

    /// Run a string of SRU source under the given source name. The text is
    /// registered as a new entry in the source map so runtime errors that
    /// happen during this evaluation render against the right text.
    pub fn eval_source_named(&mut self, name: &str, src: &str) {
        let sid = self.add_source(name, src);
        self.current_frame.source = sid;
        let mut pos = 0;
        loop {
            match crate::parser::parse_one_statement(Some(self), src, pos) {
                Ok((new_pos, Some(expr))) => {
                    let _ = crate::eval::eval_program(self, vec![expr]);
                    pos = new_pos;
                }
                Ok((_, None)) => break,
                Err(e) => panic!("{} parse error at {}: {}", name, e.pos, e.msg),
            }
        }
    }

    /// Back-compat helper — wraps `eval_source_named` with a generic name.
    pub fn eval_source(&mut self, src: &str) {
        self.eval_source_named("<eval>", src);
    }

    /// Push a new SRU frame to invoke `proc_id` with `args`. The new frame
    /// runs as the current frame; when it ends, EndOfFrame pops back and
    /// pushes the proc's return value onto the previous frame's local_stack.
    /// This is the iterative alternative to `invoke_inline` for control-flow
    /// builtins (if / while) that would otherwise grow the Rust stack.
    pub fn push_sru_frame(&mut self, proc_id: ObjId, args: Vec<ObjId>, recv: ObjId, recv_as_self: bool) -> bool {
        let (vargs, retval, body, def_bind, src) = match &self.heap.get(proc_id).data {
            Some(crate::object::ObjData::Proc(crate::object::ProcKind::Sru(p))) => (
                p.vargs.clone(),
                p.retval,
                p.body.clone(),
                p.def_binding,
                p.source,
            ),
            _ => return false,
        };
        let mut bind_args = args;
        if recv_as_self {
            bind_args.insert(0, recv);
        }
        let new_bind = self.make_binding(def_bind);
        for (i, name) in vargs.iter().enumerate() {
            let val = bind_args.get(i).copied().unwrap_or(self.builtin.nil_id);
            self.heap.set_slot(new_bind, *name, val);
        }
        if let Some(rname) = retval {
            // Make a continuation snapshot of the *current* frame so `return`
            // jumps to the caller of this push.
            let frame_clone = self.current_frame.clone();
            let frame_obj = self
                .heap
                .alloc_with_data(crate::object::ObjData::Frame(frame_clone));
            let cont = self.heap.alloc_with_data(crate::object::ObjData::Proc(
                crate::object::ProcKind::Continuation(frame_obj),
            ));
            self.heap.set_slot(new_bind, rname, cont);
        }
        let mut new_frame = crate::eval::StackFrame::new(new_bind, body);
        new_frame.source = src;
        let made_cont = retval.is_some();
        let is_tail = !made_cont
            && self.current_frame.it >= self.current_frame.operations.len()
            && self.current_frame.tree_it >= self.current_frame.expressions.len();
        if is_tail {
            let prev_upper = self.current_frame.upper.take();
            new_frame.upper = prev_upper;
            let old = std::mem::replace(&mut self.current_frame, new_frame);
            drop(old);
        } else {
            let prev = std::mem::replace(
                &mut self.current_frame,
                crate::eval::StackFrame::new(new_bind, Vec::new()),
            );
            new_frame.upper = Some(Box::new(prev));
            self.current_frame = new_frame;
        }
        true
    }

    /// Create a Binding object parented at `parent`.
    pub fn make_binding(&mut self, parent: ObjId) -> ObjId {
        let id = self.heap.alloc_empty();
        let klass_sym = symbol::intern("klass");
        let parent_sym = symbol::intern("_parent");
        self.heap.set_slot(id, klass_sym, self.builtin.binding_cls);
        if parent != self.builtin.nil_id {
            self.heap.set_slot(id, parent_sym, parent);
        }
        id
    }
}
