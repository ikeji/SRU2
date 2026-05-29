//! Bootstrap of the built-in class hierarchy and the global root binding.

use crate::object::{BasicObject, NumVal, ObjData, ObjId, ProcKind};
use crate::symbol::{self, SymbolId};
use crate::vm::Vm;

mod array;
mod boolean;
mod ffi;
mod hash;
pub mod io;
mod math;
pub mod numeric;
mod object_cls;
mod parser_obj;
mod proc;
mod string;
mod sys;

fn install_method(vm: &mut Vm, cls: ObjId, name: &str, f: crate::object::NativeFn) {
    let proc_obj = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::Native(f)));
    let im_sym = symbol::intern("instanceMethods");
    let im = vm
        .heap
        .get_slot(cls, im_sym)
        .expect("instanceMethods missing");
    let name_sym = symbol::intern(name);
    vm.heap.set_slot(im, name_sym, proc_obj);
}

fn make_class(vm: &mut Vm, name: &str, superclass: ObjId) -> ObjId {
    let cls = vm.heap.alloc(BasicObject::empty());
    let class_cls = vm.builtin.class_cls;
    vm.heap.set_slot(cls, symbol::intern("klass"), class_cls);
    vm.heap.set_slot(cls, symbol::intern("superclass"), superclass);
    // instanceMethods dict — also a BasicObject.
    let im = vm.heap.alloc(BasicObject::empty());
    vm.heap.set_slot(cls, symbol::intern("instanceMethods"), im);
    // __name as a String.
    let name_obj = vm.heap.alloc_with_data(ObjData::Str(name.to_string()));
    // (Don't set klass=String yet; circular bootstrap. Done later.)
    vm.heap.set_slot(cls, symbol::intern("__name"), name_obj);
    cls
}

pub fn bootstrap(vm: &mut Vm) {
    // ---- nil singleton ----
    let nil_id = vm.heap.alloc(BasicObject::empty());
    vm.builtin.nil_id = nil_id;

    // ---- Class & Object (mutually referenced) ----
    let class_cls = vm.heap.alloc(BasicObject::empty());
    let object_cls = vm.heap.alloc(BasicObject::empty());
    vm.builtin.class_cls = class_cls;
    vm.builtin.object_cls = object_cls;

    // Class.klass = Class (self-ref). Class.superclass = Object.
    vm.heap.set_slot(class_cls, symbol::intern("klass"), class_cls);
    vm.heap.set_slot(class_cls, symbol::intern("superclass"), object_cls);
    let class_im = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(class_cls, symbol::intern("instanceMethods"), class_im);

    // Object.klass = Class. Object.superclass = nil.
    vm.heap.set_slot(object_cls, symbol::intern("klass"), class_cls);
    vm.heap.set_slot(object_cls, symbol::intern("superclass"), nil_id);
    let object_im = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(object_cls, symbol::intern("instanceMethods"), object_im);

    let class_name = vm.heap.alloc_with_data(ObjData::Str("Class".into()));
    vm.heap.set_slot(class_cls, symbol::intern("__name"), class_name);
    let object_name = vm.heap.alloc_with_data(ObjData::Str("Object".into()));
    vm.heap.set_slot(object_cls, symbol::intern("__name"), object_name);

    // ---- Remaining built-in classes (all <: Object) ----
    let nil_cls = make_class(vm, "Nil", object_cls);
    let boolean_cls = make_class(vm, "Boolean", object_cls);
    let numeric_cls = make_class(vm, "Numeric", object_cls);
    let string_cls = make_class(vm, "String", object_cls);
    let array_cls = make_class(vm, "Array", object_cls);
    let proc_cls = make_class(vm, "Proc", object_cls);
    let binding_cls = make_class(vm, "Binding", object_cls);
    let math_cls = make_class(vm, "Math", object_cls);
    let sys_cls = make_class(vm, "Sys", object_cls);
    let hash_cls = make_class(vm, "Hash", object_cls);

    vm.builtin.nil_cls = nil_cls;
    vm.builtin.boolean_cls = boolean_cls;
    vm.builtin.numeric_cls = numeric_cls;
    vm.builtin.string_cls = string_cls;
    vm.builtin.array_cls = array_cls;
    vm.builtin.proc_cls = proc_cls;
    vm.builtin.binding_cls = binding_cls;
    vm.builtin.math_cls = math_cls;
    vm.builtin.sys_cls = sys_cls;
    vm.builtin.hash_cls = hash_cls;

    // Now set klass on nil itself.
    vm.heap.set_slot(nil_id, symbol::intern("klass"), nil_cls);

    // Fix klass on the __name Strings now that String class exists.
    let str_klass = symbol::intern("klass");
    vm.heap.set_slot(class_name, str_klass, string_cls);
    vm.heap.set_slot(object_name, str_klass, string_cls);
    for cls in [
        nil_cls, boolean_cls, numeric_cls, string_cls, array_cls, proc_cls, binding_cls, math_cls,
        sys_cls, hash_cls,
    ] {
        if let Some(n) = vm.heap.get_slot(cls, symbol::intern("__name")) {
            vm.heap.set_slot(n, str_klass, string_cls);
        }
    }

    // ---- true / false singletons ----
    let true_id = vm.heap.alloc(BasicObject::empty());
    vm.heap.set_slot(true_id, symbol::intern("klass"), boolean_cls);
    let false_id = vm.heap.alloc(BasicObject::empty());
    vm.heap.set_slot(false_id, symbol::intern("klass"), boolean_cls);
    vm.builtin.true_id = true_id;
    vm.builtin.false_id = false_id;

    // ---- Root binding ----
    let root_bind = vm.heap.alloc(BasicObject::empty());
    vm.heap.set_slot(root_bind, symbol::intern("klass"), binding_cls);
    vm.root_binding = root_bind;
    vm.current_frame.binding = root_bind;

    // Bind well-known names.
    let bindings: &[(&str, ObjId)] = &[
        ("nil", nil_id),
        ("true", true_id),
        ("false", false_id),
        ("Class", class_cls),
        ("Object", object_cls),
        ("Nil", nil_cls),
        ("Boolean", boolean_cls),
        ("Numeric", numeric_cls),
        ("String", string_cls),
        ("Array", array_cls),
        ("Proc", proc_cls),
        ("Binding", binding_cls),
        ("Math", math_cls),
        ("Sys", sys_cls),
        ("Hash", hash_cls),
    ];
    for (name, id) in bindings {
        vm.heap.set_slot(root_bind, symbol::intern(name), *id);
    }

    // Install built-in class methods (class objects themselves).
    install_class_methods(vm);

    // Install instance methods.
    boolean::install(vm);
    numeric::install(vm);
    string::install(vm);
    array::install(vm);
    object_cls::install(vm);
    proc::install(vm);
    math::install(vm);
    sys::install(vm);
    hash::install(vm);
    ffi::install(vm);

    // Install globals: p, puts, print, exit, etc.
    io::install(vm, root_bind);

    // Expose `__parser` for parser-extension hooks.
    parser_obj::install(vm);
}

fn install_class_methods(vm: &mut Vm) {
    // Class.new — directly callable on a class object.
    let new_proc = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(class_new)));
    vm.heap.set_slot(vm.builtin.class_cls, symbol::intern("new"), new_proc);
    let new_proc2 = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(class_new)));
    vm.heap.set_slot(vm.builtin.class_cls, symbol::intern("subclass"), new_proc2);

    // Per-class "new" slot (so `Foo.new(...)` works without going through
    // instance-method dispatch — csru sets `mew` on each class but we wire up
    // `new` directly here).
    let classes = [
        vm.builtin.object_cls,
        vm.builtin.numeric_cls,
        vm.builtin.string_cls,
        vm.builtin.boolean_cls,
        vm.builtin.proc_cls,
        vm.builtin.binding_cls,
        vm.builtin.nil_cls,
        vm.builtin.math_cls,
        vm.builtin.sys_cls,
    ];
    for c in classes {
        let p = vm
            .heap
            .alloc_with_data(ObjData::Proc(ProcKind::Native(class_new)));
        vm.heap.set_slot(c, symbol::intern("new"), p);
    }
    // Array.new is special — initialize an empty Array data.
    let an = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(array::array_new)));
    vm.heap.set_slot(vm.builtin.array_cls, symbol::intern("new"), an);

    // Class.subclass — for `class A < B` desugar.
    let sc = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(class_subclass)));
    vm.heap.set_slot(vm.builtin.class_cls, symbol::intern("subclass"), sc);
    // Also expose on each concrete class so `Object.subclass("Foo")` works.
    for c in classes {
        let p = vm
            .heap
            .alloc_with_data(ObjData::Proc(ProcKind::Native(class_subclass)));
        vm.heap.set_slot(c, symbol::intern("subclass"), p);
    }
}

/// `SomeClass.new(args...)` — create instance, set klass, call initialize.
fn class_new(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] is the class (the receiver).
    let cls = args[0];
    let inst = vm.heap.alloc(BasicObject::empty());
    vm.heap.set_slot(inst, symbol::intern("klass"), cls);
    // Look up initialize method on the class chain.
    let init_sym = symbol::intern("initialize");
    let init = lookup_method(vm, inst, init_sym);
    if init != vm.builtin.nil_id {
        // Call initialize(self, args[1..]).
        // We need to invoke via the eval machinery, but we're in the middle of
        // a Native call — so we can't push a new frame easily. Instead we
        // synthesize the call manually for SRU procs by setting up a sub-frame
        // and running it inline.
        let init_args: Vec<ObjId> = args[1..].to_vec();
        invoke_inline(vm, init, inst, init_args, true);
    }
    inst
}

/// `cls.subclass(name)` — make a new class whose superclass is `cls`.
pub fn class_subclass(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] = receiver class, args[1] = name (String).
    let parent = args[0];
    let new_cls = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(new_cls, symbol::intern("klass"), vm.builtin.class_cls);
    vm.heap.set_slot(new_cls, symbol::intern("superclass"), parent);
    let im = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(new_cls, symbol::intern("instanceMethods"), im);
    if let Some(name) = args.get(1) {
        vm.heap.set_slot(new_cls, symbol::intern("__name"), *name);
    }
    // Inherit `new` so `Subclass.new(...)` works.
    let new_proc = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(class_new)));
    vm.heap.set_slot(new_cls, symbol::intern("new"), new_proc);
    let sub_proc = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(class_subclass)));
    vm.heap
        .set_slot(new_cls, symbol::intern("subclass"), sub_proc);
    new_cls
}

pub fn lookup_method(vm: &Vm, recv: ObjId, method: SymbolId) -> ObjId {
    let klass_sym = symbol::intern("klass");
    let inst_sym = symbol::intern("instanceMethods");
    let super_sym = symbol::intern("superclass");
    if let Some(direct) = vm.heap.get_slot(recv, method) {
        return direct;
    }
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

/// Synchronously invoke a Proc (Native or SRU) and return the result.
/// `recv_as_self` controls whether the receiver should be prepended as the
/// first argument when binding SRU proc formals — true for instance method
/// calls (where the proc has `self` as varg[0]), false for direct proc calls.
pub fn invoke_inline(
    vm: &mut Vm,
    proc_id: ObjId,
    recv: ObjId,
    args: Vec<ObjId>,
    recv_as_self: bool,
) -> ObjId {
    let kind = {
        let bo = vm.heap.get(proc_id);
        match &bo.data {
            Some(ObjData::Proc(k)) => Some(match k {
                ProcKind::Native(_) => 0u8,
                ProcKind::Sru(_) => 2,
                ProcKind::NativeSmash(_) => 1,
                ProcKind::Continuation(_) => 3,
            }),
            _ => None,
        }
    };
    let mut bind_args = args;
    if recv_as_self {
        bind_args.insert(0, recv);
    }
    match kind {
        Some(0) => {
            let f = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::Native(f))) = &bo.data {
                    *f
                } else {
                    unreachable!()
                }
            };
            // Native convention: args[0] = recv, then user args.
            let mut full = Vec::with_capacity(bind_args.len() + 1);
            if !recv_as_self {
                full.push(recv);
            }
            full.extend(bind_args);
            let prev_proc = vm.current_native_proc;
            vm.current_native_proc = proc_id;
            let r = f(vm, &full);
            vm.current_native_proc = prev_proc;
            r
        }
        Some(2) => {
            let (vargs, retval, body, def_bind, src) = {
                let bo = vm.heap.get(proc_id);
                if let Some(ObjData::Proc(ProcKind::Sru(p))) = &bo.data {
                    (
                        p.vargs.clone(),
                        p.retval,
                        p.body.clone(),
                        p.def_binding,
                        p.source,
                    )
                } else {
                    unreachable!()
                }
            };
            let new_bind = vm.make_binding(def_bind);
            for (i, name) in vargs.iter().enumerate() {
                let val = bind_args.get(i).copied().unwrap_or(vm.builtin.nil_id);
                vm.heap.set_slot(new_bind, *name, val);
            }
            if let Some(rname) = retval {
                let cont = make_continuation(vm);
                vm.heap.set_slot(new_bind, rname, cont);
            }
            let floor = crate::eval::frame_depth_of(vm.frame()) + 1;
            let mut new_frame = crate::eval::StackFrame::new(new_bind, body);
            new_frame.source = src;
            let prev = std::mem::replace(vm.frame_mut(), crate::eval::StackFrame::new(new_bind, vec![]));
            new_frame.upper = Some(Box::new(prev));
            *vm.frame_mut() = new_frame;
            crate::eval::run_inline(vm, floor)
        }
        _ => vm.builtin.nil_id,
    }
}

fn make_continuation(vm: &mut Vm) -> ObjId {
    let frame_clone = vm.frame().clone();
    let frame_obj = vm.heap.alloc_with_data(ObjData::Frame(frame_clone));
    vm.heap
        .alloc_with_data(ObjData::Proc(ProcKind::Continuation(frame_obj)))
}

// Re-exports for use across the builtin sub-modules.
pub(crate) fn truth(vm: &Vm, b: bool) -> ObjId {
    if b {
        vm.builtin.true_id
    } else {
        vm.builtin.false_id
    }
}

pub(crate) fn nil(vm: &Vm) -> ObjId {
    vm.builtin.nil_id
}

pub(crate) fn make_num_int(vm: &mut Vm, n: i64) -> ObjId {
    let id = vm.heap.alloc_with_data(ObjData::Num(NumVal::Int(n)));
    vm.heap.set_slot(id, symbol::intern("klass"), vm.builtin.numeric_cls);
    id
}

pub(crate) fn make_num_real(vm: &mut Vm, n: f64) -> ObjId {
    let id = vm.heap.alloc_with_data(ObjData::Num(NumVal::Real(n)));
    vm.heap.set_slot(id, symbol::intern("klass"), vm.builtin.numeric_cls);
    id
}

pub(crate) fn make_str(vm: &mut Vm, s: String) -> ObjId {
    let id = vm.heap.alloc_with_data(ObjData::Str(s));
    vm.heap.set_slot(id, symbol::intern("klass"), vm.builtin.string_cls);
    id
}

pub(crate) fn as_num(vm: &Vm, id: ObjId) -> Option<NumVal> {
    if let Some(ObjData::Num(n)) = &vm.heap.get(id).data {
        Some(n.clone())
    } else {
        None
    }
}

pub(crate) fn as_str(vm: &Vm, id: ObjId) -> Option<String> {
    if let Some(ObjData::Str(s)) = &vm.heap.get(id).data {
        Some(s.clone())
    } else {
        None
    }
}

/// Like `as_num` but aborts with a typed error if the value isn't a Numeric.
/// Use in Native methods that demand a Numeric arg (e.g., `Numeric#plus`).
pub(crate) fn expect_num(vm: &Vm, id: ObjId, ctx: &str) -> NumVal {
    match as_num(vm, id) {
        Some(n) => n,
        None => crate::eval::runtime_error(
            vm,
            format!(
                "{} expected Numeric, got {}",
                ctx,
                crate::builtin::io::inspect(vm, id)
            ),
        ),
    }
}

#[allow(dead_code)]
pub(crate) fn expect_str(vm: &Vm, id: ObjId, ctx: &str) -> String {
    match as_str(vm, id) {
        Some(s) => s,
        None => crate::eval::runtime_error(
            vm,
            format!(
                "{} expected String, got {}",
                ctx,
                crate::builtin::io::inspect(vm, id)
            ),
        ),
    }
}
