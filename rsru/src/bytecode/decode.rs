//! `Module` → `Vec<Expression>`. We just re-inflate the bytecode back into
//! the AST and reuse the existing evaluator — no separate bytecode VM. This
//! keeps the AST semantics as the single source of truth and means bytecode
//! is only a compact load format, not a parallel execution model.

use crate::ast::Expression;
use crate::symbol;

use super::{unpack_pos, Insn, InsnKind, Module};

pub fn to_program(m: &Module) -> Vec<Expression> {
    decode_block(m, &m.top)
}

fn decode_block(m: &Module, ops: &[Insn]) -> Vec<Expression> {
    let mut stack: Vec<Expression> = Vec::new();
    let mut stmts: Vec<Expression> = Vec::new();
    for op in ops {
        let pos = unpack_pos(op.pos);
        match &op.kind {
            InsnKind::Lds(k) => stack.push(Expression::StrLit {
                value: m.strings[*k as usize].clone(),
                pos,
            }),
            InsnKind::Ref { var, has_env } => {
                let env = if *has_env { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Ref {
                    var: sym(m, *var),
                    env,
                    pos,
                });
            }
            InsnKind::Let { var, has_env } => {
                let value = Box::new(stack.pop().unwrap());
                let env = if *has_env { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Let {
                    var: sym(m, *var),
                    env,
                    value,
                    pos,
                });
            }
            InsnKind::Call { method, argc, has_recv } => {
                let args: Vec<Expression> = stack.split_off(stack.len() - *argc as usize);
                let receiver = if *has_recv { Some(Box::new(stack.pop().unwrap())) } else { None };
                stack.push(Expression::Call {
                    receiver,
                    method: sym(m, *method),
                    args,
                    pos,
                });
            }
            InsnKind::Proc(k) => {
                let p = &m.procs[*k as usize];
                let body = decode_block(m, &p.body);
                stack.push(Expression::Proc {
                    vargs: p.vargs.iter().map(|i| sym(m, *i)).collect(),
                    retval: p.retval.map(|i| sym(m, i)),
                    body,
                    pos,
                });
            }
            InsnKind::Pop => {
                let e = stack.pop().expect("Pop on empty stack");
                stmts.push(e);
            }
        }
    }
    if let Some(e) = stack.pop() {
        stmts.push(e);
    }
    stmts
}

fn sym(m: &Module, i: u32) -> symbol::SymbolId {
    symbol::intern(&m.symbols[i as usize])
}
