use crate::builtin::{install_method, truth};
use crate::object::{ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let b = vm.builtin.boolean_cls;
    // ifTrue/ifTrueFalse are NativeSmash so they push a frame via the main
    // eval loop instead of recursing through Rust via invoke_inline —
    // otherwise a tight while-loop overflows the Rust stack.
    install_smash(vm, b, "ifTrue", if_true_smash);
    install_smash(vm, b, "ifFalse", if_false_smash);
    install_smash(vm, b, "ifTrueFalse", if_true_false_smash);
    install_method(vm, b, "ampamp", ampamp);
    install_method(vm, b, "pipepipe", pipepipe);
    install_method(vm, b, "exclamation", excl);
    install_method(vm, b, "equal", equal);
    install_method(vm, b, "notEqual", not_equal);
}

fn install_smash(vm: &mut Vm, cls: ObjId, name: &str, f: crate::object::NativeSmashFn) {
    let proc_obj = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::NativeSmash(f)));
    let im = vm
        .heap
        .get_slot(cls, symbol::intern("instanceMethods"))
        .expect("instanceMethods missing");
    vm.heap.set_slot(im, symbol::intern(name), proc_obj);
}

fn is_true(vm: &Vm, id: ObjId) -> bool {
    id == vm.builtin.true_id
}

fn invoke_proc_smash(vm: &mut Vm, proc_id: ObjId) {
    // For Native procs (e.g., closures wrapping a builtin), fall back to
    // invoke_inline. For SRU procs, push a frame so the main eval loop
    // runs them iteratively.
    let is_sru = matches!(
        &vm.heap.get(proc_id).data,
        Some(ObjData::Proc(ProcKind::Sru(_)))
    );
    if is_sru {
        if vm.push_sru_frame(proc_id, vec![], vm.builtin.nil_id, false) {
            return;
        }
    }
    // Fallback (Native or invalid): push nil so the calling frame doesn't
    // get a missing local_stack entry.
    let r = crate::builtin::invoke_inline(vm, proc_id, vm.builtin.nil_id, vec![], false);
    vm.frame_mut().local_stack.push(r);
}

fn if_true_smash(vm: &mut Vm, args: &[ObjId]) {
    if is_true(vm, args[0]) {
        invoke_proc_smash(vm, args[1]);
    } else {
        let nil = vm.builtin.nil_id;
        vm.frame_mut().local_stack.push(nil);
    }
}

fn if_false_smash(vm: &mut Vm, args: &[ObjId]) {
    if !is_true(vm, args[0]) {
        invoke_proc_smash(vm, args[1]);
    } else {
        let nil = vm.builtin.nil_id;
        vm.frame_mut().local_stack.push(nil);
    }
}

fn if_true_false_smash(vm: &mut Vm, args: &[ObjId]) {
    if is_true(vm, args[0]) {
        invoke_proc_smash(vm, args[1]);
    } else {
        invoke_proc_smash(vm, args[2]);
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
