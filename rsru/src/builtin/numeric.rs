use crate::builtin::{as_num, install_method, make_num_int, make_num_real, make_str, truth};
use crate::object::{NumVal, ObjId};
use crate::vm::Vm;

pub fn install(vm: &mut Vm) {
    let n = vm.builtin.numeric_cls;
    install_method(vm, n, "plus", plus);
    install_method(vm, n, "minus", minus);
    install_method(vm, n, "multiply", mul);
    install_method(vm, n, "asterisk", mul);
    install_method(vm, n, "divide", div);
    install_method(vm, n, "slash", div);
    install_method(vm, n, "percent", modulo);
    install_method(vm, n, "mod", modulo);
    install_method(vm, n, "equal", equal);
    install_method(vm, n, "notEqual", not_equal);
    install_method(vm, n, "lessThan", less);
    install_method(vm, n, "lessOrEqual", le);
    install_method(vm, n, "greaterThan", greater);
    install_method(vm, n, "greaterOrEqual", ge);
    install_method(vm, n, "toS", to_s);
    install_method(vm, n, "invert", invert);
    install_method(vm, n, "floor", floor);
    install_method(vm, n, "ceil", ceil);
    install_method(vm, n, "round", round);
    install_method(vm, n, "toInt", to_int);
    install_method(vm, n, "toReal", to_real);
    // Numeric.parse as a class-level method.
    let parse_proc = vm
        .heap
        .alloc_with_data(crate::object::ObjData::Proc(crate::object::ProcKind::Native(parse)));
    vm.heap
        .set_slot(vm.builtin.numeric_cls, crate::symbol::intern("parse"), parse_proc);
}

fn binop_num<F1, F2>(vm: &mut Vm, args: &[ObjId], i_op: F1, f_op: F2) -> ObjId
where
    F1: FnOnce(i64, i64) -> i64,
    F2: FnOnce(f64, f64) -> f64,
{
    let a = as_num(vm, args[0]).unwrap();
    let b = as_num(vm, args[1]).unwrap();
    match (a, b) {
        (NumVal::Int(x), NumVal::Int(y)) => make_num_int(vm, i_op(x, y)),
        (x, y) => make_num_real(vm, f_op(as_f64(&x), as_f64(&y))),
    }
}

fn as_f64(n: &NumVal) -> f64 {
    match n {
        NumVal::Int(i) => *i as f64,
        NumVal::Real(f) => *f,
    }
}

fn plus(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    binop_num(vm, args, |a, b| a + b, |a, b| a + b)
}
fn minus(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    binop_num(vm, args, |a, b| a - b, |a, b| a - b)
}
fn mul(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    binop_num(vm, args, |a, b| a * b, |a, b| a * b)
}
fn div(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let a = as_num(vm, args[0]).unwrap();
    let b = as_num(vm, args[1]).unwrap();
    match (a, b) {
        (NumVal::Int(x), NumVal::Int(y)) => make_num_int(vm, x / y),
        (x, y) => make_num_real(vm, as_f64(&x) / as_f64(&y)),
    }
}
fn modulo(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let a = as_num(vm, args[0]).unwrap();
    let b = as_num(vm, args[1]).unwrap();
    match (a, b) {
        (NumVal::Int(x), NumVal::Int(y)) => make_num_int(vm, x % y),
        (x, y) => make_num_real(vm, as_f64(&x) % as_f64(&y)),
    }
}

fn cmp(vm: &Vm, args: &[ObjId]) -> std::cmp::Ordering {
    let a = as_num(vm, args[0]).unwrap();
    let b = as_num(vm, args[1]).unwrap();
    match (a, b) {
        (NumVal::Int(x), NumVal::Int(y)) => x.cmp(&y),
        (x, y) => as_f64(&x).partial_cmp(&as_f64(&y)).unwrap_or(std::cmp::Ordering::Equal),
    }
}

fn equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, cmp(vm, args).is_eq())
}
fn not_equal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, !cmp(vm, args).is_eq())
}
fn less(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, cmp(vm, args).is_lt())
}
fn le(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, !cmp(vm, args).is_gt())
}
fn greater(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, cmp(vm, args).is_gt())
}
fn ge(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    truth(vm, !cmp(vm, args).is_lt())
}

fn to_s(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let s = match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => i.to_string(),
        NumVal::Real(f) => format_real(f),
    };
    make_str(vm, s)
}

fn invert(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_int(vm, -i),
        NumVal::Real(f) => make_num_real(vm, -f),
    }
}

fn floor(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_int(vm, i),
        NumVal::Real(f) => make_num_int(vm, f.floor() as i64),
    }
}

fn ceil(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_int(vm, i),
        NumVal::Real(f) => make_num_int(vm, f.ceil() as i64),
    }
}

fn round(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_int(vm, i),
        NumVal::Real(f) => make_num_int(vm, f.round() as i64),
    }
}

fn to_int(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_int(vm, i),
        NumVal::Real(f) => make_num_int(vm, f as i64),
    }
}

fn to_real(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    match as_num(vm, args[0]).unwrap() {
        NumVal::Int(i) => make_num_real(vm, i as f64),
        NumVal::Real(f) => make_num_real(vm, f),
    }
}

/// Format a real to match csru's `SRUNumeric::Inspect` (C++ `ostringstream`
/// default precision = 6 significant digits, `%g`-style).
pub fn format_real(f: f64) -> String {
    if f == 0.0 {
        return "0".to_string();
    }
    if !f.is_finite() {
        return format!("{}", f);
    }
    let abs = f.abs();
    let exp = abs.log10().floor() as i32;
    // C++ `%g` switches to scientific if exponent < -4 or >= precision (6).
    if exp >= -4 && exp < 6 {
        let decimals = (5 - exp).max(0) as usize;
        let s = format!("{:.*}", decimals, f);
        trim_trailing_zeros(&s)
    } else {
        // Scientific: "1.23456e+10" with 5 digits after the decimal.
        format!("{:.5e}", f)
    }
}

fn trim_trailing_zeros(s: &str) -> String {
    if !s.contains('.') {
        return s.to_string();
    }
    let trimmed = s.trim_end_matches('0').trim_end_matches('.');
    if trimmed.is_empty() || trimmed == "-" {
        "0".to_string()
    } else {
        trimmed.to_string()
    }
}

/// Numeric.parse(receiver, str) — csru's calling convention.
fn parse(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] = Numeric class (receiver), args[1] = string.
    let s = match &vm.heap.get(args[1]).data {
        Some(crate::object::ObjData::Str(s)) => s.clone(),
        _ => return vm.builtin.nil_id,
    };
    if s.contains('.') {
        let v: f64 = s.parse().unwrap_or(0.0);
        make_num_real(vm, v)
    } else {
        let v: i64 = s.parse().unwrap_or(0);
        make_num_int(vm, v)
    }
}
