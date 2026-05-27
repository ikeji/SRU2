//! Math singleton — sin/cos/srand/rand. csru exposes these as class-level
//! procs on the `Math` object itself (not instance methods).

use crate::builtin::{make_num_int, make_num_real};
use crate::object::{NumVal, ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;
use std::cell::Cell;

thread_local! {
    static RNG_STATE: Cell<u64> = const { Cell::new(0x9E37_79B9_7F4A_7C15) };
}

fn next_rand() -> u64 {
    // xorshift64. Plenty random for SRU programs.
    RNG_STATE.with(|s| {
        let mut x = s.get();
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        s.set(x);
        x
    })
}

pub fn install(vm: &mut Vm) {
    let m = vm.builtin.math_cls;
    register(vm, m, "sin", sin);
    register(vm, m, "cos", cos);
    register(vm, m, "sqrt", sqrt);
    register(vm, m, "srand", srand);
    register(vm, m, "rand", rand);
}

fn register(vm: &mut Vm, recv: ObjId, name: &str, f: crate::object::NativeFn) {
    let p = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::Native(f)));
    vm.heap.set_slot(recv, symbol::intern(name), p);
}

fn to_f(vm: &Vm, id: ObjId) -> f64 {
    match &vm.heap.get(id).data {
        Some(ObjData::Num(NumVal::Int(i))) => *i as f64,
        Some(ObjData::Num(NumVal::Real(f))) => *f,
        _ => 0.0,
    }
}

fn sin(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    make_num_real(vm, to_f(vm, args[1]).sin())
}
fn cos(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    make_num_real(vm, to_f(vm, args[1]).cos())
}
fn sqrt(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    make_num_real(vm, to_f(vm, args[1]).sqrt())
}
fn srand(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let seed = match &vm.heap.get(args[1]).data {
        Some(ObjData::Num(NumVal::Int(i))) => *i as u64,
        Some(ObjData::Num(NumVal::Real(f))) => *f as u64,
        _ => 0,
    };
    RNG_STATE.with(|s| s.set(seed.max(1)));
    make_num_int(vm, 0)
}
fn rand(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let r = next_rand();
    match args.get(1).map(|id| &vm.heap.get(*id).data) {
        Some(Some(ObjData::Num(NumVal::Int(n)))) if *n > 0 => {
            make_num_int(vm, (r % (*n as u64)) as i64)
        }
        _ => {
            // 0 args → float in [0, 1).
            let f = (r as f64) / (u64::MAX as f64);
            make_num_real(vm, f)
        }
    }
}
