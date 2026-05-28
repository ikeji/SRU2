use crate::builtin::invoke_inline;
use crate::object::{ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let p = vm.builtin.proc_cls;
    // NativeSmash: invoke the receiver proc by pushing a new frame via the
    // main eval loop, instead of recursing through Rust via invoke_inline.
    // This is essential for the recursive while-loop desugaring not to blow
    // the Rust stack.
    let proc_obj = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::NativeSmash(call_smash)));
    let im = vm
        .heap
        .get_slot(p, symbol::intern("instanceMethods"))
        .expect("instanceMethods missing");
    vm.heap.set_slot(im, symbol::intern("call"), proc_obj);
}

fn call_smash(vm: &mut Vm, args: &[ObjId]) {
    // args[0] = the proc itself (receiver), args[1..] = passed args.
    let recv = args[0];
    let rest: Vec<ObjId> = args[1..].to_vec();
    // For SRU procs, push a frame so the main eval loop runs them
    // iteratively. For Native, invoke_inline (one Rust frame deep).
    let is_sru = matches!(
        &vm.heap.get(recv).data,
        Some(ObjData::Proc(ProcKind::Sru(_)))
    );
    if is_sru {
        let nil = vm.builtin.nil_id;
        if vm.push_sru_frame(recv, rest, nil, false) {
            return;
        }
        let r = invoke_inline(vm, recv, nil, vec![], false);
        vm.frame_mut().local_stack.push(r);
        return;
    }
    let nil = vm.builtin.nil_id;
    let r = invoke_inline(vm, recv, nil, rest, false);
    vm.frame_mut().local_stack.push(r);
}
