//! Sys singleton — Sys.clock returns process CPU time as a Numeric.

use crate::builtin::make_num_int;
use crate::object::{ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;
use std::time::Instant;

thread_local! {
    static BOOT: std::cell::OnceCell<Instant> = const { std::cell::OnceCell::new() };
}

pub fn install(vm: &mut Vm) {
    BOOT.with(|c| {
        let _ = c.set(Instant::now());
    });
    let s = vm.builtin.sys_cls;
    let p = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(clock)));
    vm.heap.set_slot(s, symbol::intern("clock"), p);
}

fn clock(vm: &mut Vm, _args: &[ObjId]) -> ObjId {
    let micros = BOOT.with(|c| c.get().map(|t| t.elapsed().as_micros() as i64).unwrap_or(0));
    make_num_int(vm, micros)
}
