use crate::builtin::{as_num, install_method, make_num_int};
use crate::object::{NumVal, ObjData, ObjId};
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let a = vm.builtin.array_cls;
    install_method(vm, a, "push", push);
    install_method(vm, a, "ltlt", push);
    install_method(vm, a, "at", at);
    install_method(vm, a, "get", at);
    install_method(vm, a, "set", set);
    install_method(vm, a, "size", size);
    install_method(vm, a, "length", size);
}

pub fn array_new(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let _ = args;
    let id = vm.heap.alloc_with_data(ObjData::Array(Vec::new()));
    vm.heap
        .set_slot(id, crate::symbol::intern("klass"), vm.builtin.array_cls);
    id
}

fn push(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        for a in &args[1..] {
            v.push(*a);
        }
    }
    recv
}

fn at(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let i = match as_num(vm, args[1]).unwrap() {
        NumVal::Int(i) => i as usize,
        NumVal::Real(f) => f as usize,
    };
    if let Some(ObjData::Array(v)) = &vm.heap.get(recv).data {
        v.get(i).copied().unwrap_or(vm.builtin.nil_id)
    } else {
        vm.builtin.nil_id
    }
}

fn set(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let i = match as_num(vm, args[1]).unwrap() {
        NumVal::Int(i) => i as usize,
        NumVal::Real(f) => f as usize,
    };
    let val = args[2];
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        if i < v.len() {
            v[i] = val;
        }
    }
    val
}

fn size(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let n = if let Some(ObjData::Array(v)) = &vm.heap.get(recv).data {
        v.len() as i64
    } else {
        0
    };
    make_num_int(vm, n)
}
