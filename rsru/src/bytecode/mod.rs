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
}

#[derive(Debug, Clone)]
pub struct ProcDef {
    pub vargs: Vec<u32>,
    pub retval: Option<u32>,
    pub body: Vec<Insn>,
}

#[derive(Debug, Clone)]
pub enum Insn {
    /// Push string constant `strings[k]`.
    Lds(u32),
    /// Look up `symbols[var]`. `has_env=true` pops the env from the stack.
    Ref { var: u32, has_env: bool },
    /// Assign top-of-stack to `symbols[var]`. `has_env=true` pops env first.
    /// Leaves the value on the stack (matches the `Let` returns its rhs
    /// rule).
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
