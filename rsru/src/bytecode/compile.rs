//! `Vec<Expression>` → `Module`. Walks the AST, interns strings and
//! symbol names into the module's pools, emits per-statement op streams.

use std::collections::HashMap;

use crate::ast::Expression;
use crate::symbol;

use super::{pack_pos, Insn, InsnKind, Module, ProcDef};

pub fn compile(stmts: &[Expression]) -> Module {
    compile_with_source(stmts, "")
}

/// Compile a program and embed the original source text into the module so
/// runtime error rendering can show line:col + snippet later.
pub fn compile_with_source(stmts: &[Expression], source: &str) -> Module {
    let mut b = Builder::new(source.to_string());
    b.emit_program(stmts);
    b.finish()
}

struct Builder {
    strings: Vec<String>,
    string_idx: HashMap<String, u32>,
    symbols: Vec<String>,
    symbol_idx: HashMap<String, u32>,
    procs: Vec<ProcDef>,
    top: Vec<Insn>,
    source: String,
}

impl Builder {
    fn new(source: String) -> Self {
        Self {
            strings: Vec::new(),
            string_idx: HashMap::new(),
            symbols: Vec::new(),
            symbol_idx: HashMap::new(),
            procs: Vec::new(),
            top: Vec::new(),
            source,
        }
    }

    fn intern_str(&mut self, s: &str) -> u32 {
        if let Some(&i) = self.string_idx.get(s) {
            return i;
        }
        let i = self.strings.len() as u32;
        self.strings.push(s.to_string());
        self.string_idx.insert(s.to_string(), i);
        i
    }

    fn intern_sym(&mut self, id: symbol::SymbolId) -> u32 {
        let name = symbol::name(id);
        if let Some(&i) = self.symbol_idx.get(&name) {
            return i;
        }
        let i = self.symbols.len() as u32;
        self.symbol_idx.insert(name.clone(), i);
        self.symbols.push(name);
        i
    }

    fn finish(self) -> Module {
        Module {
            strings: self.strings,
            symbols: self.symbols,
            procs: self.procs,
            top: self.top,
            source: self.source,
        }
    }

    fn emit_program(&mut self, stmts: &[Expression]) {
        let last = stmts.len().saturating_sub(1);
        for (i, e) in stmts.iter().enumerate() {
            let mut buf = Vec::new();
            self.emit(e, &mut buf);
            self.top.extend(buf);
            if i != last {
                self.top.push(Insn::new(InsnKind::Pop, super::WIRE_POS_UNKNOWN));
            }
        }
    }

    fn emit(&mut self, e: &Expression, out: &mut Vec<Insn>) {
        let pos = pack_pos(e.pos());
        match e {
            Expression::StrLit { value, .. } => {
                let k = self.intern_str(value);
                out.push(Insn::new(InsnKind::Lds(k), pos));
            }
            Expression::Ref { var, env, .. } => {
                let has_env = env.is_some();
                if let Some(e) = env {
                    self.emit(e, out);
                }
                let v = self.intern_sym(*var);
                out.push(Insn::new(InsnKind::Ref { var: v, has_env }, pos));
            }
            Expression::Let { var, env, value, .. } => {
                if let Some(e) = env {
                    self.emit(e, out);
                }
                self.emit(value, out);
                let v = self.intern_sym(*var);
                out.push(Insn::new(
                    InsnKind::Let {
                        var: v,
                        has_env: env.is_some(),
                    },
                    pos,
                ));
            }
            Expression::Call { receiver, method, args, .. } => {
                if let Some(r) = receiver {
                    self.emit(r, out);
                }
                for a in args {
                    self.emit(a, out);
                }
                let m = self.intern_sym(*method);
                out.push(Insn::new(
                    InsnKind::Call {
                        method: m,
                        argc: args.len() as u32,
                        has_recv: receiver.is_some(),
                    },
                    pos,
                ));
            }
            Expression::Proc { vargs, retval, body, .. } => {
                let body_last = body.len().saturating_sub(1);
                let mut body_buf = Vec::new();
                for (i, s) in body.iter().enumerate() {
                    let mut tmp = Vec::new();
                    self.emit(s, &mut tmp);
                    body_buf.extend(tmp);
                    if i != body_last {
                        body_buf.push(Insn::new(InsnKind::Pop, super::WIRE_POS_UNKNOWN));
                    }
                }
                let vargs_idx: Vec<u32> = vargs.iter().map(|s| self.intern_sym(*s)).collect();
                let retval_idx = retval.map(|s| self.intern_sym(s));
                let proc_idx = self.procs.len() as u32;
                self.procs.push(ProcDef {
                    vargs: vargs_idx,
                    retval: retval_idx,
                    body: body_buf,
                });
                out.push(Insn::new(InsnKind::Proc(proc_idx), pos));
            }
        }
    }
}
