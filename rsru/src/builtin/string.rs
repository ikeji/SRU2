use crate::builtin::{as_num, as_str, install_method, make_num_int, make_str, truth};
use crate::object::{NumVal, ObjId};
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let s = vm.builtin.string_cls;
    install_method(vm, s, "plus", plus);
    install_method(vm, s, "equal", equal);
    install_method(vm, s, "notEqual", not_equal);
    install_method(vm, s, "size", size);
    install_method(vm, s, "length", size);
    install_method(vm, s, "get", get);
    install_method(vm, s, "at", get);
    install_method(vm, s, "substr", substr);
    install_method(vm, s, "toS", to_s);
    install_method(vm, s, "indexOf", index_of);
    install_method(vm, s, "split", split);
    install_method(vm, s, "upcase", upcase);
    install_method(vm, s, "downcase", downcase);
    install_method(vm, s, "trim", trim);
}

fn plus(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let a = as_str(vm, args[0]).unwrap_or_default();
    let b = as_str(vm, args[1]).unwrap_or_default();
    make_str(vm, format!("{}{}", a, b))
}

fn equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, as_str(vm, args[0]) == as_str(vm, args[1]))
}
fn not_equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, as_str(vm, args[0]) != as_str(vm, args[1]))
}

fn size(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    make_num_int(vm, s.len() as i64)
}

fn get(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    let i = match crate::builtin::expect_num(vm, args[1], "String index") {
        NumVal::Int(i) => i as usize,
        NumVal::Real(f) => f as usize,
    };
    if i >= s.len() {
        make_str(vm, String::new())
    } else {
        make_str(vm, s[i..i + 1].to_string())
    }
}

fn substr(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    let start = match crate::builtin::expect_num(vm, args[1], "String#substr start") {
        NumVal::Int(i) => i as usize,
        NumVal::Real(f) => f as usize,
    };
    if start >= s.len() {
        return make_str(vm, String::new());
    }
    let end = if args.len() > 2 {
        let n = match crate::builtin::expect_num(vm, args[2], "String#substr len") {
            NumVal::Int(i) => i as usize,
            NumVal::Real(f) => f as usize,
        };
        (start + n).min(s.len())
    } else {
        s.len()
    };
    make_str(vm, s[start..end].to_string())
}

fn to_s(_vm: &mut Vm, args: &[ObjId]) -> ObjId {
    args[0]
}

/// `s.indexOf(needle)` — first byte index of `needle` in `s`, or -1.
fn index_of(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let hay = as_str(vm, args[0]).unwrap_or_default();
    let needle = as_str(vm, args[1]).unwrap_or_default();
    let idx = hay
        .find(needle.as_str())
        .map(|n| n as i64)
        .unwrap_or(-1);
    make_num_int(vm, idx)
}

fn upcase(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    make_str(vm, s.to_uppercase())
}

fn downcase(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    make_str(vm, s.to_lowercase())
}

fn trim(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = as_str(vm, args[0]).unwrap_or_default();
    make_str(vm, s.trim().to_string())
}

/// `s.split(sep)` — split on `sep` and return an Array of Strings. Empty
/// separator returns the original wrapped in a 1-element array.
fn split(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let hay = as_str(vm, args[0]).unwrap_or_default();
    let sep = as_str(vm, args[1]).unwrap_or_default();
    let parts: Vec<String> = if sep.is_empty() {
        vec![hay]
    } else {
        hay.split(sep.as_str()).map(str::to_string).collect()
    };
    let mut ids = Vec::with_capacity(parts.len());
    for p in parts {
        ids.push(make_str(vm, p));
    }
    let arr = vm.heap.alloc_with_data(crate::object::ObjData::Array(ids));
    vm.heap.set_slot(
        arr,
        crate::symbol::intern("klass"),
        vm.builtin.array_cls,
    );
    arr
}
