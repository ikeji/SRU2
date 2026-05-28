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
    register(vm, s, "clock", clock);
    register(vm, s, "gc", gc);
    register(vm, s, "liveCount", live_count);
}

fn register(vm: &mut Vm, recv: ObjId, name: &str, f: crate::object::NativeFn) {
    let p = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::Native(f)));
    vm.heap.set_slot(recv, symbol::intern(name), p);
}

fn clock(vm: &mut Vm, _args: &[ObjId]) -> ObjId {
    let micros = BOOT.with(|c| c.get().map(|t| t.elapsed().as_micros() as i64).unwrap_or(0));
    make_num_int(vm, micros)
}

/// `Sys.gc()` — force a mark-and-sweep collection now. Returns the number
/// of live BasicObjects in the heap after the collection.
fn gc(vm: &mut Vm, _args: &[ObjId]) -> ObjId {
    crate::gc::collect(vm);
    let live = vm.heap.live_count() as i64;
    make_num_int(vm, live)
}

/// `Sys.liveCount()` — number of live BasicObjects without forcing GC.
fn live_count(vm: &mut Vm, _args: &[ObjId]) -> ObjId {
    let live = vm.heap.live_count() as i64;
    make_num_int(vm, live)
}
