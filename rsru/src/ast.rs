use crate::symbol::SymbolId;

/// Source-byte offset attached to every AST node and to every TraceOp.
/// A value of `usize::MAX` means "no known position" (e.g., synthesised by
/// the prelude desugar or by tests). Use `Pos::UNKNOWN` for that case.
pub type Pos = usize;
pub const POS_UNKNOWN: Pos = usize::MAX;

#[derive(Debug, Clone)]
pub enum Expression {
    Let {
        var: SymbolId,
        env: Option<Box<Expression>>,
        value: Box<Expression>,
        pos: Pos,
    },
    Ref {
        var: SymbolId,
        env: Option<Box<Expression>>,
        pos: Pos,
    },
    Call {
        receiver: Option<Box<Expression>>,
        method: SymbolId,
        args: Vec<Expression>,
        pos: Pos,
    },
    Proc {
        vargs: Vec<SymbolId>,
        retval: Option<SymbolId>,
        body: Vec<Expression>,
        pos: Pos,
    },
    StrLit {
        value: String,
        pos: Pos,
    },
}

impl Expression {
    pub fn pos(&self) -> Pos {
        match self {
            Expression::Let { pos, .. }
            | Expression::Ref { pos, .. }
            | Expression::Call { pos, .. }
            | Expression::Proc { pos, .. }
            | Expression::StrLit { pos, .. } => *pos,
        }
    }
}
