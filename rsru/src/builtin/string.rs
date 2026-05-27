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
    let i = match as_num(vm, args[1]).unwrap() {
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
    let start = match as_num(vm, args[1]).unwrap() {
        NumVal::Int(i) => i as usize,
        NumVal::Real(f) => f as usize,
    };
    if start >= s.len() {
        return make_str(vm, String::new());
    }
    let end = if args.len() > 2 {
        let n = match as_num(vm, args[2]).unwrap() {
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
