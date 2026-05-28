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
    register(vm, s, "readFile", read_file);
    register(vm, s, "writeFile", write_file);
    register(vm, s, "appendFile", append_file);
    register(vm, s, "fileExists", file_exists);
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

fn arg_str(vm: &Vm, id: ObjId) -> Option<String> {
    if let Some(crate::object::ObjData::Str(s)) = &vm.heap.get(id).data {
        Some(s.clone())
    } else {
        None
    }
}

/// `Sys.readFile(path)` — read entire file as String, or nil on error.
fn read_file(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let path = match arg_str(vm, args[1]) {
        Some(p) => p,
        None => return vm.builtin.nil_id,
    };
    match std::fs::read_to_string(&path) {
        Ok(s) => crate::builtin::make_str(vm, s),
        Err(_) => vm.builtin.nil_id,
    }
}

/// `Sys.writeFile(path, content)` — write content as the whole file. Returns
/// true on success, false on error.
fn write_file(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let path = match arg_str(vm, args[1]) {
        Some(p) => p,
        None => return vm.builtin.false_id,
    };
    let content = match arg_str(vm, args[2]) {
        Some(c) => c,
        None => return vm.builtin.false_id,
    };
    if std::fs::write(&path, content).is_ok() {
        vm.builtin.true_id
    } else {
        vm.builtin.false_id
    }
}

/// `Sys.appendFile(path, content)` — append content to a file (creating
/// it if needed). Returns true on success, false on error.
fn append_file(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    use std::io::Write;
    let path = match arg_str(vm, args[1]) {
        Some(p) => p,
        None => return vm.builtin.false_id,
    };
    let content = match arg_str(vm, args[2]) {
        Some(c) => c,
        None => return vm.builtin.false_id,
    };
    let res = std::fs::OpenOptions::new()
        .append(true)
        .create(true)
        .open(&path)
        .and_then(|mut f| f.write_all(content.as_bytes()));
    if res.is_ok() { vm.builtin.true_id } else { vm.builtin.false_id }
}

fn file_exists(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let path = match arg_str(vm, args[1]) {
        Some(p) => p,
        None => return vm.builtin.false_id,
    };
    if std::path::Path::new(&path).exists() {
        vm.builtin.true_id
    } else {
        vm.builtin.false_id
    }
}
