//! `Module` → `Vec<Expression>`. We just re-inflate the bytecode back into
//! the AST and reuse the existing evaluator — no separate bytecode VM. This
//! keeps the AST semantics as the single source of truth and means bytecode
//! is only a compact load format, not a parallel execution model.

use crate::ast::Expression;
use crate::symbol;

use super::{Insn, Module};

pub fn to_program(m: &Module) -> Vec<Expression> {
    decode_block(m, &m.top)
}

fn decode_block(m: &Module, ops: &[Insn]) -> Vec<Expression> {
    // Walk ops with a tiny stack-machine; each statement-sized expression
    // ends with a `Pop`, or — for the final statement — with the end of the
    // op list.
    let mut stack: Vec<Expression> = Vec::new();
    let mut stmts: Vec<Expression> = Vec::new();
    let p_unknown = crate::ast::POS_UNKNOWN;
    for op in ops {
        match op {
            Insn::Lds(k) => stack.push(Expression::StrLit {
                value: m.strings[*k as usize].clone(),
                pos: p_unknown,
            }),
            Insn::Ref { var, has_env } => {
                let env = if *has_env { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Ref {
                    var: sym(m, *var),
                    env,
                    pos: p_unknown,
                });
            }
            Insn::Let { var, has_env } => {
                let value = Box::new(stack.pop().unwrap());
                let env = if *has_env { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Let {
                    var: sym(m, *var),
                    env,
                    value,
                    pos: p_unknown,
                });
            }
            Insn::Call { method, argc, has_recv } => {
                let args: Vec<Expression> = stack.split_off(stack.len() - *argc as usize);
                let receiver = if *has_recv { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Call {
                    receiver,
                    method: sym(m, *method),
                    args,
                    pos: p_unknown,
                });
            }
            Insn::Proc(k) => {
                let p = &m.procs[*k as usize];
                let body = decode_block(m, &p.body);
                stack.push(Expression::Proc {
                    vargs: p.vargs.iter().map(|i| sym(m, *i)).collect(),
                    retval: p.retval.map(|i| sym(m, i)),
                    body,
                    pos: p_unknown,
                });
            }
            Insn::Pop => {
                let e = stack.pop().expect("Pop on empty stack");
                stmts.push(e);
            }
        }
    }
    // Final statement (no trailing Pop).
    if let Some(e) = stack.pop() {
        stmts.push(e);
    }
    stmts
}

fn sym(m: &Module, i: u32) -> symbol::SymbolId {
    symbol::intern(&m.symbols[i as usize])
}
