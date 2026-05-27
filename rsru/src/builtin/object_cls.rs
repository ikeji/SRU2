use crate::builtin::{install_method, truth};
use crate::object::ObjId;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let o = vm.builtin.object_cls;
    install_method(vm, o, "equal", equal);
    install_method(vm, o, "notEqual", not_equal);
    install_method(vm, o, "class", class);
}

fn equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, args[0] == args[1])
}

fn not_equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, args[0] != args[1])
}

fn class(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    vm.heap
        .get_slot(args[0], crate::symbol::intern("klass"))
        .unwrap_or(vm.builtin.nil_id)
}
