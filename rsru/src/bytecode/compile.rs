//! `Vec<Expression>` → `Module`. Walks the AST, interns strings and
//! symbol names into the module's pools, emits per-statement op streams.

use std::collections::HashMap;

use crate::ast::Expression;
use crate::symbol;

use super::{Insn, Module, ProcDef};

pub fn compile(stmts: &[Expression]) -> Module {
    let mut b = Builder::new();
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
}

impl Builder {
    fn new() -> Self {
        Self {
            strings: Vec::new(),
            string_idx: HashMap::new(),
            symbols: Vec::new(),
            symbol_idx: HashMap::new(),
            procs: Vec::new(),
            top: Vec::new(),
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
        }
    }

    fn emit_program(&mut self, stmts: &[Expression]) {
        let last = stmts.len().saturating_sub(1);
        for (i, e) in stmts.iter().enumerate() {
            let mut buf = Vec::new();
            self.emit(e, &mut buf);
            self.top.extend(buf);
            if i != last {
                self.top.push(Insn::Pop);
            }
        }
    }

    fn emit(&mut self, e: &Expression, out: &mut Vec<Insn>) {
        match e {
            Expression::StrLit(s) => {
                let k = self.intern_str(s);
                out.push(Insn::Lds(k));
            }
            Expression::Ref { var, env } => {
                let has_env = env.is_some();
                if let Some(e) = env {
                    self.emit(e, out);
                }
                let v = self.intern_sym(*var);
                out.push(Insn::Ref { var: v, has_env });
            }
            Expression::Let { var, env, value } => {
                if let Some(e) = env {
                    self.emit(e, out);
                }
                self.emit(value, out);
                let v = self.intern_sym(*var);
                out.push(Insn::Let {
                    var: v,
                    has_env: env.is_some(),
                });
            }
            Expression::Call { receiver, method, args } => {
                if let Some(r) = receiver {
                    self.emit(r, out);
                }
                for a in args {
                    self.emit(a, out);
                }
                let m = self.intern_sym(*method);
                out.push(Insn::Call {
                    method: m,
                    argc: args.len() as u32,
                    has_recv: receiver.is_some(),
                });
            }
            Expression::Proc { vargs, retval, body } => {
                // Compile body into its own instruction list, recursively.
                let body_last = body.len().saturating_sub(1);
                let mut body_buf = Vec::new();
                for (i, s) in body.iter().enumerate() {
                    let mut tmp = Vec::new();
                    self.emit(s, &mut tmp);
                    body_buf.extend(tmp);
                    if i != body_last {
                        body_buf.push(Insn::Pop);
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
                out.push(Insn::Proc(proc_idx));
            }
        }
    }
}
