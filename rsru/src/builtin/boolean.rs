use crate::builtin::{install_method, truth, invoke_inline};
use crate::object::ObjId;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let b = vm.builtin.boolean_cls;
    install_method(vm, b, "ifTrue", if_true);
    install_method(vm, b, "ifFalse", if_false);
    install_method(vm, b, "ifTrueFalse", if_true_false);
    install_method(vm, b, "ampamp", ampamp);
    install_method(vm, b, "pipepipe", pipepipe);
    install_method(vm, b, "exclamation", excl);
    install_method(vm, b, "equal", equal);
    install_method(vm, b, "notEqual", not_equal);
}

fn is_true(vm: &Vm, id: ObjId) -> bool {
    id == vm.builtin.true_id
}

fn if_true(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] = self (Boolean), args[1] = thenProc.
    if is_true(vm, args[0]) {
        invoke_inline(vm, args[1], vm.builtin.nil_id, vec![], false)
    } else {
        vm.builtin.nil_id
    }
}

fn if_false(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    if !is_true(vm, args[0]) {
        invoke_inline(vm, args[1], vm.builtin.nil_id, vec![], false)
    } else {
        vm.builtin.nil_id
    }
}

fn if_true_false(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    if is_true(vm, args[0]) {
        invoke_inline(vm, args[1], vm.builtin.nil_id, vec![], false)
    } else {
        invoke_inline(vm, args[2], vm.builtin.nil_id, vec![], false)
    }
}

fn ampamp(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, is_true(vm, args[0]) && is_true(vm, args[1]))
}

fn pipepipe(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, is_true(vm, args[0]) || is_true(vm, args[1]))
}

fn excl(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, !is_true(vm, args[0]))
}

fn equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, args[0] == args[1])
}

fn not_equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, args[0] != args[1])
}
