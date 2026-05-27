//! `__parser` object — exposes the parser as a slot-dispatched SRU object.
//! SRU code can override `__parser.const_literal` (and similar) to extend the
//! grammar at runtime, mirroring csru's `__parser.<rule>` model.

use crate::ast::Expression;
use crate::builtin::{make_num_int, make_str};
use crate::object::{BasicObject, NumVal, ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

pub fn install(vm: &mut Vm) -> ObjId {
    let parser = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(parser, symbol::intern("klass"), vm.builtin.object_cls);
    let cl = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(default_const_literal)));
    vm.heap
        .set_slot(parser, symbol::intern("const_literal"), cl);
    let pf = vm
        .heap
        .alloc_with_data(ObjData::Proc(ProcKind::Native(default_parse)));
    vm.heap.set_slot(parser, symbol::intern("parse"), pf);
    vm.builtin.parser_id = parser;
    // Bind in root binding.
    let bind = vm.root_binding;
    vm.heap
        .set_slot(bind, symbol::intern("__parser"), parser);
    parser
}

/// `__parser.const_literal(zelf, src, pos)` — try to parse a real, number, or
/// string starting at `pos` in `src`. Returns a true-result `{status:true,
/// pos:end, ast:Expr}` on success or `{status:false, pos:pos}` on failure.
///
/// Args layout: every Native receives `[recv, ...explicit]`. With csru-style
/// calling (`self.const_literal(self, src, pos)`), the explicit args are
/// `[zelf, src, pos]`, so the Native sees `[recv, zelf, src, pos]` and reads
/// from index 2 onward.
fn default_const_literal(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let src_id = match args.get(2) {
        Some(id) => *id,
        None => return make_false_result(vm, 0, "missing src arg"),
    };
    let pos_id = match args.get(3) {
        Some(id) => *id,
        None => return make_false_result(vm, 0, "missing pos arg"),
    };
    let src = as_string(vm, src_id).unwrap_or_default();
    let pos = as_usize(vm, pos_id).unwrap_or(0);
    let bytes = src.as_bytes();
    if let Some((end, expr)) = scan_const_literal(bytes, pos) {
        make_true_result(vm, end, expr)
    } else {
        make_false_result(vm, pos, "no literal")
    }
}

/// `__parser.parse(src)` — parse `src` as one statement and return
/// `{status, pos, ast}`. `ast` holds an `ObjData::Expr`.
///
/// Args layout: called as `__parser.parse(src)` so `args = [recv, src]` and
/// we read index 1.
fn default_parse(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let src_id = match args.get(1) {
        Some(id) => *id,
        None => return make_false_result(vm, 0, "missing src arg"),
    };
    let src = as_string(vm, src_id).unwrap_or_default();
    let r = crate::parser::parse_one_statement(Some(vm), &src, 0);
    match r {
        Ok((new_pos, Some(expr))) => make_true_result(vm, new_pos, expr),
        Ok((new_pos, None)) => make_false_result(vm, new_pos, "empty source"),
        Err(e) => make_false_result(vm, e.pos, "parse error"),
    }
}

pub fn make_true_result(vm: &mut Vm, pos: usize, expr: Expression) -> ObjId {
    let out = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(out, symbol::intern("klass"), vm.builtin.object_cls);
    let true_id = vm.builtin.true_id;
    vm.heap.set_slot(out, symbol::intern("status"), true_id);
    let pos_obj = make_num_int(vm, pos as i64);
    vm.heap.set_slot(out, symbol::intern("pos"), pos_obj);
    let ast_obj = vm.heap.alloc_with_data(ObjData::Expr(expr));
    vm.heap.set_slot(out, symbol::intern("ast"), ast_obj);
    out
}

pub fn make_false_result(vm: &mut Vm, pos: usize, msg: &str) -> ObjId {
    let out = vm.heap.alloc(BasicObject::empty());
    vm.heap
        .set_slot(out, symbol::intern("klass"), vm.builtin.object_cls);
    let false_id = vm.builtin.false_id;
    vm.heap.set_slot(out, symbol::intern("status"), false_id);
    let pos_obj = make_num_int(vm, pos as i64);
    vm.heap.set_slot(out, symbol::intern("pos"), pos_obj);
    let err = make_str(vm, msg.to_string());
    vm.heap.set_slot(out, symbol::intern("error"), err);
    out
}

fn as_string(vm: &Vm, id: ObjId) -> Option<String> {
    if let Some(ObjData::Str(s)) = &vm.heap.get(id).data {
        Some(s.clone())
    } else {
        None
    }
}

fn as_usize(vm: &Vm, id: ObjId) -> Option<usize> {
    match &vm.heap.get(id).data {
        Some(ObjData::Num(NumVal::Int(i))) => Some(*i as usize),
        Some(ObjData::Num(NumVal::Real(f))) => Some(*f as usize),
        _ => None,
    }
}

fn scan_const_literal(bytes: &[u8], pos: usize) -> Option<(usize, Expression)> {
    if bytes.get(pos) == Some(&b'"') {
        return scan_string(bytes, pos);
    }
    if bytes.get(pos).map_or(false, |c| c.is_ascii_digit()) {
        return scan_number(bytes, pos);
    }
    None
}

fn scan_string(bytes: &[u8], start: usize) -> Option<(usize, Expression)> {
    let mut pos = start + 1;
    let mut buf = String::new();
    while pos < bytes.len() {
        match bytes[pos] {
            b'"' => return Some((pos + 1, Expression::StrLit(buf))),
            b'\\' => {
                pos += 1;
                if pos >= bytes.len() {
                    return None;
                }
                match bytes[pos] {
                    b'n' => buf.push('\n'),
                    b'r' => buf.push('\r'),
                    b'\\' => buf.push('\\'),
                    b'"' => buf.push('"'),
                    c => buf.push(c as char),
                }
                pos += 1;
            }
            c => {
                buf.push(c as char);
                pos += 1;
            }
        }
    }
    None
}

fn scan_number(bytes: &[u8], start: usize) -> Option<(usize, Expression)> {
    let mut pos = start;
    let mut has_dot = false;
    while pos < bytes.len() {
        let c = bytes[pos];
        if c.is_ascii_digit() {
            pos += 1;
        } else if c == b'.' && !has_dot {
            if pos + 1 < bytes.len() && bytes[pos + 1].is_ascii_digit() {
                has_dot = true;
                pos += 1;
                continue;
            }
            break;
        } else {
            break;
        }
    }
    if pos == start {
        return None;
    }
    let s = std::str::from_utf8(&bytes[start..pos]).ok()?.to_string();
    let call = Expression::Call {
        receiver: Some(Box::new(Expression::Ref {
            var: symbol::intern("Numeric"),
            env: None,
        })),
        method: symbol::intern("parse"),
        args: vec![Expression::StrLit(s)],
    };
    Some((pos, call))
}
