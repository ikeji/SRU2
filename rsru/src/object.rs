use std::collections::HashMap;

use crate::ast::Expression;
use crate::eval::StackFrame;
use crate::symbol::SymbolId;

#[derive(Copy, Clone, Debug, PartialEq, Eq, Hash, Default)]
pub struct ObjId(pub u32);

#[derive(Debug)]
pub struct BasicObject {
    pub fields: HashMap<SymbolId, ObjId>,
    pub data: Option<ObjData>,
}

impl BasicObject {
    pub fn empty() -> Self {
        Self {
            fields: HashMap::new(),
            data: None,
        }
    }

    pub fn with_data(data: ObjData) -> Self {
        Self {
            fields: HashMap::new(),
            data: Some(data),
        }
    }
}

#[derive(Debug, Clone)]
pub enum NumVal {
    Int(i64),
    Real(f64),
}

#[derive(Debug)]
pub enum ObjData {
    Num(NumVal),
    Str(String),
    Array(Vec<ObjId>),
    Proc(ProcKind),
    Frame(StackFrame),
}

#[derive(Debug)]
pub enum ProcKind {
    Sru(SruProc),
    Native(NativeFn),
    NativeSmash(NativeSmashFn),
    Continuation(ObjId), // index of a Frame BasicObject
}

#[derive(Debug, Clone)]
pub struct SruProc {
    pub vargs: Vec<SymbolId>,
    pub retval: Option<SymbolId>,
    pub body: Vec<Expression>,
    pub def_binding: ObjId,
}

pub type NativeFn = fn(&mut crate::vm::Vm, &[ObjId]) -> ObjId;
pub type NativeSmashFn = fn(&mut crate::vm::Vm, &[ObjId]);
