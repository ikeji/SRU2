//! Sample rsru native plugin. Uses ONLY the C-ABI bridge exposed by rsru
//! through `SruContext` — no direct rsru-lib dependency. The struct layout
//! below must match `rsru::builtin::ffi::SruContext` byte-for-byte.

#![allow(improper_ctypes_definitions)]

use std::os::raw::c_void;

type SruObj = u32;
type SruVm = *mut c_void;

pub type SruNativeFn = extern "C" fn(ctx: *const SruContext, args: *const SruObj, argc: usize) -> SruObj;

#[repr(C)]
pub struct SruContext {
    pub vm: SruVm,
    pub root_binding: SruObj,
    pub nil: SruObj,
    pub true_obj: SruObj,
    pub false_obj: SruObj,

    pub intern: extern "C" fn(SruVm, *const u8, usize) -> u32,
    pub alloc_int: extern "C" fn(SruVm, i64) -> SruObj,
    pub alloc_real: extern "C" fn(SruVm, f64) -> SruObj,
    pub alloc_string: extern "C" fn(SruVm, *const u8, usize) -> SruObj,

    pub as_int: extern "C" fn(SruVm, SruObj, *mut i64) -> u8,
    pub as_real: extern "C" fn(SruVm, SruObj, *mut f64) -> u8,
    pub as_string: extern "C" fn(SruVm, SruObj, *mut *const u8, *mut usize) -> u8,

    pub set_slot: extern "C" fn(SruVm, SruObj, u32, SruObj),
    pub get_slot: extern "C" fn(SruVm, SruObj, u32) -> SruObj,
    pub make_native: extern "C" fn(SruVm, SruNativeFn) -> SruObj,
}

fn ctx_intern(ctx: &SruContext, name: &str) -> u32 {
    (ctx.intern)(ctx.vm, name.as_ptr(), name.len())
}

fn bind_global(ctx: &SruContext, name: &str, f: SruNativeFn) {
    let sym = ctx_intern(ctx, name);
    let proc_obj = (ctx.make_native)(ctx.vm, f);
    (ctx.set_slot)(ctx.vm, ctx.root_binding, sym, proc_obj);
}

#[no_mangle]
pub extern "C" fn sru_require(ctx: *const SruContext) -> bool {
    let ctx = unsafe { &*ctx };
    bind_global(ctx, "pluginHello", plugin_hello);
    bind_global(ctx, "pluginAdd", plugin_add);
    true
}

extern "C" fn plugin_hello(
    ctx: *const SruContext,
    _args: *const SruObj,
    _argc: usize,
) -> SruObj {
    let ctx = unsafe { &*ctx };
    let msg = b"hello from native plugin";
    (ctx.alloc_string)(ctx.vm, msg.as_ptr(), msg.len())
}

extern "C" fn plugin_add(
    ctx: *const SruContext,
    args: *const SruObj,
    argc: usize,
) -> SruObj {
    let ctx = unsafe { &*ctx };
    if argc < 3 {
        return ctx.nil;
    }
    let args = unsafe { std::slice::from_raw_parts(args, argc) };
    // args[0]=receiver (binding), args[1]=a, args[2]=b
    let mut a_i: i64 = 0;
    let mut b_i: i64 = 0;
    let mut a_f: f64 = 0.0;
    let mut b_f: f64 = 0.0;
    let a_is_int = (ctx.as_int)(ctx.vm, args[1], &mut a_i) != 0;
    let b_is_int = (ctx.as_int)(ctx.vm, args[2], &mut b_i) != 0;
    if a_is_int && b_is_int {
        return (ctx.alloc_int)(ctx.vm, a_i + b_i);
    }
    let a_ok = a_is_int || (ctx.as_real)(ctx.vm, args[1], &mut a_f) != 0;
    let b_ok = b_is_int || (ctx.as_real)(ctx.vm, args[2], &mut b_f) != 0;
    if !a_ok || !b_ok {
        return ctx.nil;
    }
    let a = if a_is_int { a_i as f64 } else { a_f };
    let b = if b_is_int { b_i as f64 } else { b_f };
    (ctx.alloc_real)(ctx.vm, a + b)
}
