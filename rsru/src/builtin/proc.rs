use crate::builtin::{install_method, invoke_inline};
use crate::object::ObjId;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let p = vm.builtin.proc_cls;
    install_method(vm, p, "call", call);
}

fn call(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] = the proc itself, args[1..] = passed args.
    let recv = args[0];
    let rest: Vec<ObjId> = args[1..].to_vec();
    invoke_inline(vm, recv, vm.builtin.nil_id, rest, false)
}
