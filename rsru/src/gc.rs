//! Mark-and-sweep collector. Runs at safe points (top of `eval::step`) so
//! Native code's Rust-local ObjIds — which aren't reachable from any frame
//! while a Native is executing — don't get mistaken for garbage.

use crate::eval::StackFrame;
use crate::object::{ObjData, ObjId, ProcKind};
use crate::vm::Vm;

pub fn collect(vm: &mut Vm) {
    let mut work: Vec<ObjId> = Vec::with_capacity(256);

    // Roots: well-known builtin ids + root binding + every frame in the
    // active upper-chain.
    push_roots(vm, &mut work);

    // Mark phase.
    while let Some(id) = work.pop() {
        if !vm.heap.mark(id) {
            continue; // already marked
        }
        mark_children(vm, id, &mut work);
    }

    vm.heap.sweep();
}

fn push_roots(vm: &Vm, out: &mut Vec<ObjId>) {
    let b = &vm.builtin;
    for id in [
        b.nil_id,
        b.true_id,
        b.false_id,
        b.class_cls,
        b.object_cls,
        b.nil_cls,
        b.boolean_cls,
        b.numeric_cls,
        b.string_cls,
        b.array_cls,
        b.proc_cls,
        b.binding_cls,
        b.math_cls,
        b.sys_cls,
        b.parser_id,
    ] {
        out.push(id);
    }
    out.push(vm.root_binding);
    push_frame_roots(&vm.current_frame, out);
}

fn push_frame_roots(frame: &StackFrame, out: &mut Vec<ObjId>) {
    let mut cur = Some(frame);
    while let Some(f) = cur {
        out.push(f.binding);
        for id in &f.local_stack {
            out.push(*id);
        }
        cur = f.upper.as_deref();
    }
}

fn mark_children(vm: &Vm, id: ObjId, out: &mut Vec<ObjId>) {
    let bo = vm.heap.get(id);
    for v in bo.fields.values() {
        out.push(*v);
    }
    if let Some(data) = &bo.data {
        match data {
            ObjData::Num(_) | ObjData::Str(_) => {}
            ObjData::Array(items) => {
                for v in items {
                    out.push(*v);
                }
            }
            ObjData::Frame(f) => push_frame_roots(f, out),
            ObjData::Proc(p) => match p {
                ProcKind::Sru(sp) => out.push(sp.def_binding),
                ProcKind::Continuation(fid) => out.push(*fid),
                ProcKind::Native(_) | ProcKind::NativeSmash(_) => {}
            },
            ObjData::Expr(_) => {
                // Expression AST doesn't contain heap ObjIds.
            }
        }
    }
}
