//! Bytecode form of an SRU program. AST and bytecode map 1:1 so the only
//! reason to use bytecode is to skip the parser when shipping pre-compiled
//! `.sru` content (e.g., the prelude, or user-distributed libraries).
//!
//! The five SRU AST node types (Let, Ref, Call, Proc, StrLit) become five
//! opcodes, plus a `Pop` control op the trace planner already uses.

pub mod compile;
pub mod decode;
pub mod encode;

/// A self-contained compiled SRU program.
#[derive(Debug, Clone)]
pub struct Module {
    /// String constants used by `Lds`.
    pub strings: Vec<String>,
    /// Symbol names used by `Ref`, `Let`, `Call` and proc vargs / retval.
    pub symbols: Vec<String>,
    /// Closure bodies. `Proc(k)` makes a proc whose `vargs / retval / body`
    /// come from `procs[k]`.
    pub procs: Vec<ProcDef>,
    /// Top-level instruction list.
    pub top: Vec<Insn>,
    /// Original SRU source text. Embedded so runtime error rendering can
    /// still show the source line + caret even when the program is loaded
    /// from a `.bc` file. May be empty if the producer didn't include it.
    pub source: String,
}

#[derive(Debug, Clone)]
pub struct ProcDef {
    pub vargs: Vec<u32>,
    pub retval: Option<u32>,
    pub body: Vec<Insn>,
}

/// Wire form: u32 byte offset, with `u32::MAX` standing in for
/// `POS_UNKNOWN` so the on-disk format stays compact.
pub type WirePos = u32;
pub const WIRE_POS_UNKNOWN: WirePos = u32::MAX;

/// Translate runtime `Pos` (usize) to the wire form, clamping out-of-range
/// values (or `POS_UNKNOWN`) to the sentinel.
pub fn pack_pos(p: crate::ast::Pos) -> WirePos {
    if p == crate::ast::POS_UNKNOWN {
        WIRE_POS_UNKNOWN
    } else {
        p.min(u32::MAX as usize - 1) as u32
    }
}

pub fn unpack_pos(p: WirePos) -> crate::ast::Pos {
    if p == WIRE_POS_UNKNOWN {
        crate::ast::POS_UNKNOWN
    } else {
        p as usize
    }
}

#[derive(Debug, Clone)]
pub struct Insn {
    pub kind: InsnKind,
    pub pos: WirePos,
}

impl Insn {
    pub fn new(kind: InsnKind, pos: WirePos) -> Self {
        Self { kind, pos }
    }
}

#[derive(Debug, Clone)]
pub enum InsnKind {
    /// Push string constant `strings[k]`.
    Lds(u32),
    /// Look up `symbols[var]`. `has_env=true` pops the env from the stack.
    Ref { var: u32, has_env: bool },
    /// Assign top-of-stack to `symbols[var]`. `has_env=true` pops env first.
    Let { var: u32, has_env: bool },
    /// Call `symbols[method]` with `argc` args. If `has_recv`, the receiver
    /// sits below the args; otherwise the binding is the implicit receiver.
    Call {
        method: u32,
        argc: u32,
        has_recv: bool,
    },
    /// Push a closure built from `procs[k]`.
    Proc(u32),
    /// Discard top of stack — emitted between statements.
    Pop,
}
