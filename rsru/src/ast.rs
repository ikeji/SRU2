use crate::symbol::SymbolId;

#[derive(Debug, Clone)]
pub enum Expression {
    Let {
        var: SymbolId,
        env: Option<Box<Expression>>,
        value: Box<Expression>,
    },
    Ref {
        var: SymbolId,
        env: Option<Box<Expression>>,
    },
    Call {
        receiver: Option<Box<Expression>>,
        method: SymbolId,
        args: Vec<Expression>,
    },
    Proc {
        vargs: Vec<SymbolId>,
        retval: Option<SymbolId>,
        body: Vec<Expression>,
    },
    StrLit(String),
}
