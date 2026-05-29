//! `requireNative(path)` — open a shared library and call its
//! `sru_require(ctx: *const SruContext) -> bool` entry point.
//!
//! All interaction between the plugin and rsru goes through the
//! function-pointer table in `SruContext`. This keeps the plugin from
//! depending on rsru's internals (no Rust statics shared across the
//! dylib boundary). Plugins can be written in any language that emits a
//! cdylib with `sru_require` — Rust, C, Zig, etc.

use std::ffi::CString;
use std::os::raw::{c_char, c_int, c_void};
use std::ptr;

use crate::object::{NumVal, ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

#[cfg(unix)]
const RTLD_NOW: c_int = 2;

#[cfg(unix)]
extern "C" {
    fn dlopen(filename: *const c_char, flags: c_int) -> *mut c_void;
    fn dlsym(handle: *mut c_void, symbol: *const c_char) -> *mut c_void;
    fn dlerror() -> *const c_char;
}

/// C-ABI bridge passed to plugins.  Plugins read function pointers from
/// here to operate on the running VM.  Object IDs (`u32`) are opaque from
/// the plugin's perspective — it just round-trips them.
#[repr(C)]
pub struct SruContext {
    pub vm: *mut Vm,
    pub root_binding: u32,
    pub nil: u32,
    pub true_obj: u32,
    pub false_obj: u32,

    pub intern: extern "C" fn(*mut Vm, *const u8, usize) -> u32,
    pub alloc_int: extern "C" fn(*mut Vm, i64) -> u32,
    pub alloc_real: extern "C" fn(*mut Vm, f64) -> u32,
    pub alloc_string: extern "C" fn(*mut Vm, *const u8, usize) -> u32,

    /// Read `obj` as Int. Writes to `out` and returns 1 on success, 0 if
    /// the object isn't a Numeric Int.
    pub as_int: extern "C" fn(*mut Vm, u32, *mut i64) -> u8,
    pub as_real: extern "C" fn(*mut Vm, u32, *mut f64) -> u8,
    /// Read a string. Sets `*ptr` to the bytes (NOT null-terminated, NOT
    /// owned by the caller — valid until next allocation) and `*len` to
    /// the byte length. Returns 1 on success, 0 if not a String.
    pub as_string: extern "C" fn(*mut Vm, u32, *mut *const u8, *mut usize) -> u8,

    pub set_slot: extern "C" fn(*mut Vm, u32, u32, u32),
    pub get_slot: extern "C" fn(*mut Vm, u32, u32) -> u32,
    /// Wrap a `SruNativeFn` as a Proc object that can be invoked from SRU.
    pub make_native: extern "C" fn(*mut Vm, SruNativeFn) -> u32,
}

/// Plugin-side native function signature.  Args[0] is the receiver (or
/// binding when called without one); args[1..] are the SRU args.
pub type SruNativeFn =
    extern "C" fn(ctx: *const SruContext, args: *const u32, argc: usize) -> u32;

pub fn install(vm: &mut Vm) {
    let bind = vm.root_binding;
    let p = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(require_native)));
    vm.heap
        .set_slot(bind, symbol::intern("requireNative"), p);
}

fn build_ctx(vm: &mut Vm) -> SruContext {
    SruContext {
        vm: vm as *mut Vm,
        root_binding: vm.root_binding.0,
        nil: vm.builtin.nil_id.0,
        true_obj: vm.builtin.true_id.0,
        false_obj: vm.builtin.false_id.0,
        intern: ctx_intern,
        alloc_int: ctx_alloc_int,
        alloc_real: ctx_alloc_real,
        alloc_string: ctx_alloc_string,
        as_int: ctx_as_int,
        as_real: ctx_as_real,
        as_string: ctx_as_string,
        set_slot: ctx_set_slot,
        get_slot: ctx_get_slot,
        make_native: ctx_make_native,
    }
}

extern "C" fn ctx_intern(_vm: *mut Vm, name: *const u8, len: usize) -> u32 {
    let s = unsafe { std::slice::from_raw_parts(name, len) };
    let s = match std::str::from_utf8(s) {
        Ok(s) => s,
        Err(_) => return u32::MAX,
    };
    symbol::intern(s)
}

extern "C" fn ctx_alloc_int(vm: *mut Vm, v: i64) -> u32 {
    let vm = unsafe { &mut *vm };
    let id = vm.heap.alloc_with_data(ObjData::Num(NumVal::Int(v)));
    vm.heap
        .set_slot(id, symbol::intern("klass"), vm.builtin.numeric_cls);
    id.0
}

extern "C" fn ctx_alloc_real(vm: *mut Vm, v: f64) -> u32 {
    let vm = unsafe { &mut *vm };
    let id = vm.heap.alloc_with_data(ObjData::Num(NumVal::Real(v)));
    vm.heap
        .set_slot(id, symbol::intern("klass"), vm.builtin.numeric_cls);
    id.0
}

extern "C" fn ctx_alloc_string(vm: *mut Vm, ptr: *const u8, len: usize) -> u32 {
    let vm = unsafe { &mut *vm };
    let s = unsafe { std::slice::from_raw_parts(ptr, len) };
    let s = String::from_utf8_lossy(s).into_owned();
    let id = vm.heap.alloc_with_data(ObjData::Str(s));
    vm.heap
        .set_slot(id, symbol::intern("klass"), vm.builtin.string_cls);
    id.0
}

extern "C" fn ctx_as_int(vm: *mut Vm, obj: u32, out: *mut i64) -> u8 {
    let vm = unsafe { &*vm };
    match &vm.heap.get(ObjId(obj)).data {
        Some(ObjData::Num(NumVal::Int(i))) => {
            unsafe { *out = *i };
            1
        }
        _ => 0,
    }
}

extern "C" fn ctx_as_real(vm: *mut Vm, obj: u32, out: *mut f64) -> u8 {
    let vm = unsafe { &*vm };
    match &vm.heap.get(ObjId(obj)).data {
        Some(ObjData::Num(NumVal::Real(f))) => {
            unsafe { *out = *f };
            1
        }
        Some(ObjData::Num(NumVal::Int(i))) => {
            unsafe { *out = *i as f64 };
            1
        }
        _ => 0,
    }
}

extern "C" fn ctx_as_string(
    vm: *mut Vm,
    obj: u32,
    out_ptr: *mut *const u8,
    out_len: *mut usize,
) -> u8 {
    let vm = unsafe { &*vm };
    match &vm.heap.get(ObjId(obj)).data {
        Some(ObjData::Str(s)) => {
            unsafe {
                *out_ptr = s.as_ptr();
                *out_len = s.len();
            }
            1
        }
        _ => 0,
    }
}

extern "C" fn ctx_set_slot(vm: *mut Vm, obj: u32, sym: u32, value: u32) {
    let vm = unsafe { &mut *vm };
    vm.heap.set_slot(ObjId(obj), sym, ObjId(value));
}

extern "C" fn ctx_get_slot(vm: *mut Vm, obj: u32, sym: u32) -> u32 {
    let vm = unsafe { &*vm };
    vm.heap
        .get_slot(ObjId(obj), sym)
        .map(|i| i.0)
        .unwrap_or(vm.builtin.nil_id.0)
}

extern "C" fn ctx_make_native(vm: *mut Vm, f: SruNativeFn) -> u32 {
    let vm = unsafe { &mut *vm };
    let proc_obj = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(c_trampoline)));
    // Encode the fn pointer as a Numeric Int and store it in a hidden
    // slot. We can't reuse the ObjId slot encoding for non-Obj values —
    // GC would walk them as heap references and crash.
    let raw = f as usize as i64;
    let fn_box = vm.heap.alloc_with_data(ObjData::Num(NumVal::Int(raw)));
    vm.heap
        .set_slot(proc_obj, symbol::intern("__cfn"), fn_box);
    vm.heap
        .set_slot(proc_obj, symbol::intern("klass"), vm.builtin.proc_cls);
    proc_obj.0
}

fn c_trampoline(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // The dispatcher set `vm.current_native_proc` to this proc's id just
    // before calling us. Read the fn pointer from its `__cfn` slot.
    let proc_obj = vm.current_native_proc;
    let fn_box = match vm.heap.get_slot(proc_obj, symbol::intern("__cfn")) {
        Some(id) => id,
        None => return vm.builtin.nil_id,
    };
    let raw = match &vm.heap.get(fn_box).data {
        Some(ObjData::Num(NumVal::Int(i))) => *i as usize,
        _ => return vm.builtin.nil_id,
    };
    let f: SruNativeFn = unsafe { std::mem::transmute::<usize, SruNativeFn>(raw) };

    let ctx = build_ctx(vm);
    let u32_args: Vec<u32> = args.iter().map(|i| i.0).collect();
    let result = f(&ctx as *const _, u32_args.as_ptr(), u32_args.len());
    ObjId(result)
}

/// Plugin entry signature: `extern "C" fn sru_require(ctx: *const SruContext) -> bool`.
type RequireFn = extern "C" fn(*const SruContext) -> bool;

fn require_native(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let path = args.get(1).copied().unwrap_or(vm.builtin.nil_id);
    let path = match &vm.heap.get(path).data {
        Some(ObjData::Str(s)) => s.clone(),
        _ => return vm.builtin.false_id,
    };
    #[cfg(unix)]
    {
        let c_path = match CString::new(path.clone()) {
            Ok(s) => s,
            Err(_) => return vm.builtin.false_id,
        };
        let handle = unsafe { dlopen(c_path.as_ptr(), RTLD_NOW) };
        if handle.is_null() {
            let msg = unsafe {
                let p = dlerror();
                if p.is_null() {
                    "dlopen failed".to_string()
                } else {
                    std::ffi::CStr::from_ptr(p).to_string_lossy().into_owned()
                }
            };
            eprintln!("requireNative: {}: {}", path, msg);
            return vm.builtin.false_id;
        }
        let c_sym = CString::new("sru_require").unwrap();
        let sym = unsafe { dlsym(handle, c_sym.as_ptr()) };
        if sym.is_null() {
            eprintln!("requireNative: {} has no `sru_require` symbol", path);
            return vm.builtin.false_id;
        }
        let f: RequireFn = unsafe { std::mem::transmute(sym) };
        // Intentionally don't dlclose; function pointers must stay valid.
        let _ = handle;
        let ctx = build_ctx(vm);
        let ok = f(&ctx as *const _);
        if ok {
            vm.builtin.true_id
        } else {
            vm.builtin.false_id
        }
    }
    #[cfg(not(unix))]
    {
        eprintln!("requireNative: not supported on this platform");
        let _ = path;
        let _ = ptr::null::<u8>();
        vm.builtin.false_id
    }
}
