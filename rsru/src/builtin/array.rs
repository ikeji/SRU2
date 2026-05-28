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
    install_method(vm, a, "pop", pop);
    install_method(vm, a, "shift", shift);
    install_method(vm, a, "clear", clear);
    install_method(vm, a, "concat", concat);
    install_method(vm, a, "sort", sort);
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
    let i = match crate::builtin::expect_num(vm, args[1], "Array index") {
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
    let i = match crate::builtin::expect_num(vm, args[1], "Array index") {
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

fn pop(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let nil = vm.builtin.nil_id;
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        return v.pop().unwrap_or(nil);
    }
    nil
}

fn shift(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let nil = vm.builtin.nil_id;
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        if v.is_empty() { return nil; }
        return v.remove(0);
    }
    nil
}

fn clear(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        v.clear();
    }
    recv
}

fn concat(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let other = args[1];
    let to_append: Vec<ObjId> = if let Some(ObjData::Array(v)) = &vm.heap.get(other).data {
        v.clone()
    } else {
        return recv;
    };
    if let Some(ObjData::Array(v)) = &mut vm.heap.get_mut(recv).data {
        v.extend(to_append);
    }
    recv
}

/// Sort by Numeric value, or by String lexicographically. Returns a new
/// Array — does not mutate `self`. Mixed-type arrays are sorted by
/// (type-tag, value); not particularly useful but never panics.
fn sort(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let items: Vec<ObjId> = if let Some(ObjData::Array(v)) = &vm.heap.get(recv).data {
        v.clone()
    } else {
        return vm.builtin.nil_id;
    };
    let mut indexed: Vec<(SortKey, ObjId)> = items
        .into_iter()
        .map(|id| (sort_key(vm, id), id))
        .collect();
    indexed.sort_by(|a, b| a.0.cmp(&b.0));
    let sorted: Vec<ObjId> = indexed.into_iter().map(|(_, id)| id).collect();
    let out = vm.heap.alloc_with_data(ObjData::Array(sorted));
    vm.heap
        .set_slot(out, crate::symbol::intern("klass"), vm.builtin.array_cls);
    out
}

#[derive(PartialEq, Eq, PartialOrd, Ord)]
enum SortKey {
    /// Any Numeric, encoded as an ordered f64-bit pattern so Int and Real
    /// sort together correctly.
    Num(NumKey),
    Str(String),
    Other(u32),
}

#[derive(PartialEq, Eq, PartialOrd, Ord)]
struct NumKey(i64);

fn num_key(f: f64) -> NumKey {
    let bits = if f.is_nan() { f64::INFINITY.to_bits() } else { f.to_bits() };
    let i = bits as i64;
    NumKey(if i < 0 { !i } else { i ^ (1 << 63) })
}

fn sort_key(vm: &crate::vm::Vm, id: ObjId) -> SortKey {
    match &vm.heap.get(id).data {
        Some(ObjData::Num(NumVal::Int(i))) => SortKey::Num(num_key(*i as f64)),
        Some(ObjData::Num(NumVal::Real(f))) => SortKey::Num(num_key(*f)),
        Some(ObjData::Str(s)) => SortKey::Str(s.clone()),
        _ => SortKey::Other(id.0),
    }
}
