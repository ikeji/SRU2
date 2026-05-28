//! Hash class — a string/integer-keyed map. csru only has an empty
//! placeholder for Hash; rsru fills it in.

use std::collections::HashMap;

use crate::builtin::{install_method, make_num_int};
use crate::object::{HashKey, NumVal, ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let h = vm.builtin.hash_cls;
    install_method(vm, h, "set", set);
    install_method(vm, h, "get", get);
    install_method(vm, h, "at", get);
    install_method(vm, h, "hasKey", has_key);
    install_method(vm, h, "delete", delete);
    install_method(vm, h, "size", size);
    install_method(vm, h, "length", size);
    install_method(vm, h, "keys", keys);
    install_method(vm, h, "values", values);
    install_method(vm, h, "merge", merge);
    install_method(vm, h, "clear", clear);

    // Hash.new returns an empty hash. Class-level method, attached to the
    // class object directly.
    let p = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(new)));
    vm.heap.set_slot(h, symbol::intern("new"), p);
}

pub fn new(vm: &mut Vm, _args: &[ObjId]) -> ObjId {
    let id = vm.heap.alloc_with_data(ObjData::Hash(HashMap::new()));
    vm.heap
        .set_slot(id, symbol::intern("klass"), vm.builtin.hash_cls);
    id
}

fn key_of(vm: &Vm, id: ObjId, ctx: &str) -> HashKey {
    match &vm.heap.get(id).data {
        Some(ObjData::Num(NumVal::Int(i))) => HashKey::Int(*i),
        Some(ObjData::Num(NumVal::Real(f))) if f.fract() == 0.0 && f.is_finite() => {
            HashKey::Int(*f as i64)
        }
        Some(ObjData::Str(s)) => HashKey::Str(s.clone()),
        _ => crate::eval::runtime_error(
            vm,
            format!(
                "{} requires an Int or String key, got {}",
                ctx,
                crate::builtin::io::inspect(vm, id)
            ),
        ),
    }
}

fn set(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let key = key_of(vm, args[1], "Hash#set");
    let val = args[2];
    if let Some(ObjData::Hash(m)) = &mut vm.heap.get_mut(recv).data {
        m.insert(key, val);
    }
    val
}

fn get(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let key = key_of(vm, args[1], "Hash#get");
    if let Some(ObjData::Hash(m)) = &vm.heap.get(recv).data {
        return m.get(&key).copied().unwrap_or(vm.builtin.nil_id);
    }
    vm.builtin.nil_id
}

fn has_key(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let key = key_of(vm, args[1], "Hash#hasKey");
    let found = if let Some(ObjData::Hash(m)) = &vm.heap.get(recv).data {
        m.contains_key(&key)
    } else {
        false
    };
    if found { vm.builtin.true_id } else { vm.builtin.false_id }
}

fn delete(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let key = key_of(vm, args[1], "Hash#delete");
    if let Some(ObjData::Hash(m)) = &mut vm.heap.get_mut(recv).data {
        return m.remove(&key).unwrap_or(vm.builtin.nil_id);
    }
    vm.builtin.nil_id
}

fn size(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let n = if let Some(ObjData::Hash(m)) = &vm.heap.get(recv).data {
        m.len() as i64
    } else {
        0
    };
    make_num_int(vm, n)
}

fn keys(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let keys: Vec<HashKey> = if let Some(ObjData::Hash(m)) = &vm.heap.get(recv).data {
        m.keys().cloned().collect()
    } else {
        return vm.builtin.nil_id;
    };
    // Materialise each key as a SRU value, then collect into an Array.
    let mut out_ids = Vec::with_capacity(keys.len());
    for k in keys {
        let v = match k {
            HashKey::Int(i) => make_num_int(vm, i),
            HashKey::Str(s) => crate::builtin::make_str(vm, s),
        };
        out_ids.push(v);
    }
    let arr = vm.heap.alloc_with_data(ObjData::Array(out_ids));
    vm.heap
        .set_slot(arr, symbol::intern("klass"), vm.builtin.array_cls);
    arr
}

fn merge(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let other = args[1];
    let entries: Vec<(HashKey, ObjId)> =
        if let Some(ObjData::Hash(m)) = &vm.heap.get(other).data {
            m.iter().map(|(k, v)| (k.clone(), *v)).collect()
        } else {
            return recv;
        };
    if let Some(ObjData::Hash(m)) = &mut vm.heap.get_mut(recv).data {
        for (k, v) in entries {
            m.insert(k, v);
        }
    }
    recv
}

fn clear(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    if let Some(ObjData::Hash(m)) = &mut vm.heap.get_mut(recv).data {
        m.clear();
    }
    recv
}

fn values(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let recv = args[0];
    let vals: Vec<ObjId> = if let Some(ObjData::Hash(m)) = &vm.heap.get(recv).data {
        m.values().copied().collect()
    } else {
        return vm.builtin.nil_id;
    };
    let arr = vm.heap.alloc_with_data(ObjData::Array(vals));
    vm.heap
        .set_slot(arr, symbol::intern("klass"), vm.builtin.array_cls);
    arr
}
