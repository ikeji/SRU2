//! Hand-written recursive-descent parser for SRU.

use crate::ast::Expression;
use crate::symbol::{self, SymbolId};

#[derive(Debug)]
pub struct ParseError {
    pub msg: String,
    pub pos: usize,
}

pub fn parse(src: &str) -> Result<Vec<Expression>, ParseError> {
    let mut p = Parser::new(src);
    p.parse_program()
}

const RESERVED: &[&str] = &[
    "if", "elsif", "else", "then", "end", "while", "def", "class",
];

struct Parser<'a> {
    src: &'a [u8],
    pos: usize,
}

impl<'a> Parser<'a> {
    fn new(src: &'a str) -> Self {
        Self {
            src: src.as_bytes(),
            pos: 0,
        }
    }

    fn err<T>(&self, msg: &str) -> Result<T, ParseError> {
        Err(ParseError {
            msg: msg.to_string(),
            pos: self.pos,
        })
    }

    fn eos(&self) -> bool {
        self.pos >= self.src.len()
    }

    fn peek(&self) -> Option<u8> {
        self.src.get(self.pos).copied()
    }

    fn peek_at(&self, off: usize) -> Option<u8> {
        self.src.get(self.pos + off).copied()
    }

    /// Consume spaces / tabs only.
    fn spc(&mut self) {
        while matches!(self.peek(), Some(b' ' | b'\t')) {
            self.pos += 1;
        }
    }

    /// Consume spaces/tabs plus `#`-to-EOL comments (but not newlines).
    fn spc_or_comment(&mut self) {
        loop {
            self.spc();
            if self.peek() == Some(b'#') {
                while !self.eos() && self.peek() != Some(b'\n') {
                    self.pos += 1;
                }
            } else {
                return;
            }
        }
    }

    /// Consume spaces/tabs/newlines plus `#`-comments.
    fn spc_or_lf(&mut self) {
        loop {
            match self.peek() {
                Some(b' ' | b'\t' | b'\r' | b'\n') => self.pos += 1,
                Some(b'#') => {
                    while !self.eos() && self.peek() != Some(b'\n') {
                        self.pos += 1;
                    }
                }
                _ => return,
            }
        }
    }

    /// Try to consume a `\n` or `\r\n` or `\r`. Returns true if consumed.
    fn try_lf(&mut self) -> bool {
        match self.peek() {
            Some(b'\n') => {
                self.pos += 1;
                true
            }
            Some(b'\r') => {
                self.pos += 1;
                if self.peek() == Some(b'\n') {
                    self.pos += 1;
                }
                true
            }
            _ => false,
        }
    }

    fn try_eat(&mut self, s: &str) -> bool {
        let bytes = s.as_bytes();
        if self.pos + bytes.len() > self.src.len() {
            return false;
        }
        if &self.src[self.pos..self.pos + bytes.len()] != bytes {
            return false;
        }
        self.pos += bytes.len();
        true
    }

    /// Try to eat a keyword, but only if it's followed by a non-identifier char.
    fn try_keyword(&mut self, s: &str) -> bool {
        let save = self.pos;
        if !self.try_eat(s) {
            return false;
        }
        if let Some(c) = self.peek() {
            if is_ident_cont(c) {
                self.pos = save;
                return false;
            }
        }
        true
    }

    /// `[a-zA-Z_][a-zA-Z0-9_!]*`, with reserved-word check.
    fn try_ident(&mut self) -> Option<String> {
        let save = self.pos;
        let first = self.peek()?;
        if !is_ident_start(first) {
            return None;
        }
        let mut end = self.pos + 1;
        while let Some(c) = self.src.get(end).copied() {
            if is_ident_cont(c) {
                // Handle "!=" terminator: if we hit '!' followed by '=', stop
                // before the '!'.
                if c == b'!' && self.src.get(end + 1) == Some(&b'=') {
                    break;
                }
                end += 1;
            } else {
                break;
            }
        }
        let name = std::str::from_utf8(&self.src[self.pos..end]).ok()?.to_string();
        // Reserved-word rejection — but only for whole-token matches.
        if RESERVED.contains(&name.as_str()) {
            self.pos = save;
            return None;
        }
        self.pos = end;
        Some(name)
    }

    fn parse_program(&mut self) -> Result<Vec<Expression>, ParseError> {
        let mut out = Vec::new();
        loop {
            self.spc_or_lf();
            if self.eos() {
                break;
            }
            let e = self.parse_statement()?;
            out.push(e);
            self.spc_or_comment();
            // Consume statement terminator (lf, ';', or EOS).
            if self.try_lf() {
                continue;
            }
            if self.try_eat(";") {
                continue;
            }
            if self.eos() {
                break;
            }
            return self.err("expected statement terminator");
        }
        Ok(out)
    }

    /// Statements until `end`/`else`/`elsif`.
    fn parse_statements(&mut self, stop: &[&str]) -> Result<Vec<Expression>, ParseError> {
        let mut out = Vec::new();
        loop {
            self.spc_or_lf();
            for kw in stop {
                let save = self.pos;
                if self.try_keyword(kw) {
                    self.pos = save;
                    return Ok(out);
                }
            }
            if self.eos() {
                return Ok(out);
            }
            let e = self.parse_statement()?;
            out.push(e);
            self.spc_or_comment();
            if !self.try_lf() && !self.try_eat(";") {
                // Maybe followed by stop keyword on the same line.
                self.spc_or_comment();
                let mut at_stop = false;
                for kw in stop {
                    let save = self.pos;
                    if self.try_keyword(kw) {
                        self.pos = save;
                        at_stop = true;
                        break;
                    }
                }
                if at_stop || self.eos() {
                    return Ok(out);
                }
                return self.err("expected statement terminator");
            }
        }
    }

    fn parse_statement(&mut self) -> Result<Expression, ParseError> {
        self.spc_or_lf();
        // if / while / class / def
        let save = self.pos;
        if self.try_keyword("if") {
            return self.parse_if();
        }
        if self.try_keyword("while") {
            return self.parse_while();
        }
        if self.try_keyword("class") {
            return self.parse_class();
        }
        if self.try_keyword("def") {
            return self.parse_def();
        }
        self.pos = save;
        // Otherwise, parse an expression and check for `=` (let).
        let lhs = self.parse_expression()?;
        let save2 = self.pos;
        self.spc();
        if self.peek() == Some(b'=') && self.peek_at(1) != Some(b'=') {
            self.pos += 1;
            self.spc_or_lf();
            let rhs = self.parse_statement()?;
            return Ok(make_let_from(lhs, rhs));
        }
        self.pos = save2;
        Ok(lhs)
    }

    fn parse_if(&mut self) -> Result<Expression, ParseError> {
        self.spc_or_lf();
        let cond = self.parse_expression()?;
        // optional `then`, `;`, or `\n`
        self.spc_or_comment();
        let _ = self.try_keyword("then") || self.try_eat(";") || self.try_lf();
        let then_body = self.parse_statements(&["elsif", "else", "end"])?;
        let then_proc = make_proc(vec![], None, then_body);
        // Handle elsif / else / end.
        self.spc_or_lf();
        if self.try_keyword("elsif") {
            let else_branch = self.parse_if()?;
            // cond.ifTrueFalse({then}, {else})
            let else_proc = make_proc(vec![], None, vec![else_branch]);
            return Ok(make_call_method(
                cond,
                "ifTrueFalse",
                vec![then_proc, else_proc],
            ));
        }
        if self.try_keyword("else") {
            let else_body = self.parse_statements(&["end"])?;
            self.spc_or_lf();
            self.try_keyword("end");
            let else_proc = make_proc(vec![], None, else_body);
            return Ok(make_call_method(
                cond,
                "ifTrueFalse",
                vec![then_proc, else_proc],
            ));
        }
        self.try_keyword("end");
        Ok(make_call_method(cond, "ifTrue", vec![then_proc]))
    }

    fn parse_while(&mut self) -> Result<Expression, ParseError> {
        self.spc_or_lf();
        if !self.try_eat("(") {
            return self.err("`while` requires `(`");
        }
        self.spc_or_lf();
        let cond = self.parse_statement()?;
        self.spc_or_lf();
        if !self.try_eat(")") {
            return self.err("`while` requires `)`");
        }
        self.spc_or_lf();
        let body = self.parse_statements(&["end"])?;
        self.spc_or_lf();
        if !self.try_keyword("end") {
            return self.err("`while` requires `end`");
        }
        // Desugar to:
        // {|:return| loop = { if cond then body; loop() else nil end }; loop() }()
        // Actually simpler: a recursive proc with return cont.
        // We use a self-call closure via a name in current binding.
        // For simplicity, emit:
        //   __w = { if (cond) { body; __w() } else { nil } }; __w()
        Ok(make_while_desugar(cond, body))
    }

    fn parse_class(&mut self) -> Result<Expression, ParseError> {
        self.spc_or_lf();
        let name = self
            .try_ident()
            .ok_or_else(|| self.make_err("class name expected"))?;
        self.spc_or_lf();
        let parent = if self.try_eat("<") {
            self.spc_or_lf();
            Some(self.parse_expression()?)
        } else {
            None
        };
        self.spc_or_lf();
        // Parse method defs until `end`.
        let mut methods: Vec<(String, Vec<String>, Vec<Expression>)> = Vec::new();
        loop {
            self.spc_or_lf();
            if self.try_keyword("end") {
                break;
            }
            if !self.try_keyword("def") {
                return self.err("expected `def` or `end` in class body");
            }
            self.spc_or_lf();
            let mname = self
                .try_ident()
                .ok_or_else(|| self.make_err("method name expected"))?;
            self.spc_or_lf();
            if !self.try_eat("(") {
                return self.err("`(` expected after method name");
            }
            let mut vargs = vec!["self".to_string()];
            self.spc_or_lf();
            if !self.try_eat(")") {
                loop {
                    self.spc_or_lf();
                    let an = self
                        .try_ident()
                        .ok_or_else(|| self.make_err("arg name expected"))?;
                    vargs.push(an);
                    self.spc_or_lf();
                    if self.try_eat(",") {
                        continue;
                    }
                    break;
                }
                self.spc_or_lf();
                if !self.try_eat(")") {
                    return self.err("`)` expected");
                }
            }
            self.spc_or_lf();
            let body = self.parse_statements(&["end"])?;
            self.spc_or_lf();
            if !self.try_keyword("end") {
                return self.err("`end` expected for def");
            }
            methods.push((mname, vargs, body));
        }
        // Desugar to:
        //   Name = ({
        //     __cls = (parent or Object).subclass("Name")
        //     __cls.instanceMethods.method1 = { |self, ...:return| body }
        //     ...
        //     __cls
        //   })()
        // The temporary `__cls` keeps the class object reachable while we
        // attach methods, and the outer Let assigns the result to `Name` in
        // the caller's binding.
        let parent_expr = parent.unwrap_or_else(|| ref_expr("Object"));
        let cls_sym = symbol::intern("__cls");
        let mut stmts: Vec<Expression> = Vec::new();
        stmts.push(Expression::Let {
            var: cls_sym,
            env: None,
            value: Box::new(Expression::Call {
                receiver: Some(Box::new(parent_expr)),
                method: symbol::intern("subclass"),
                args: vec![Expression::StrLit(name.clone())],
            }),
        });
        for (mname, vargs, body) in methods {
            let vargs_ids: Vec<SymbolId> = vargs.iter().map(|s| symbol::intern(s)).collect();
            let proc = Expression::Proc {
                vargs: vargs_ids,
                retval: Some(symbol::intern("return")),
                body,
            };
            // __cls.instanceMethods.<mname> = proc
            let im_ref = Expression::Ref {
                var: symbol::intern("instanceMethods"),
                env: Some(Box::new(Expression::Ref { var: cls_sym, env: None })),
            };
            stmts.push(Expression::Let {
                var: symbol::intern(&mname),
                env: Some(Box::new(im_ref)),
                value: Box::new(proc),
            });
        }
        stmts.push(Expression::Ref { var: cls_sym, env: None });
        let proc = Expression::Proc {
            vargs: vec![],
            retval: None,
            body: stmts,
        };
        let class_value = Expression::Call {
            receiver: Some(Box::new(proc)),
            method: symbol::intern("call"),
            args: vec![],
        };
        Ok(Expression::Let {
            var: symbol::intern(&name),
            env: None,
            value: Box::new(class_value),
        })
    }

    fn parse_def(&mut self) -> Result<Expression, ParseError> {
        self.spc_or_lf();
        let name = self
            .try_ident()
            .ok_or_else(|| self.make_err("def name expected"))?;
        self.spc_or_lf();
        if !self.try_eat("(") {
            return self.err("`(` expected after def name");
        }
        let mut vargs = Vec::new();
        self.spc_or_lf();
        if !self.try_eat(")") {
            loop {
                self.spc_or_lf();
                let an = self
                    .try_ident()
                    .ok_or_else(|| self.make_err("arg name expected"))?;
                vargs.push(an);
                self.spc_or_lf();
                if self.try_eat(",") {
                    continue;
                }
                break;
            }
            self.spc_or_lf();
            if !self.try_eat(")") {
                return self.err("`)` expected");
            }
        }
        self.spc_or_lf();
        let body = self.parse_statements(&["end"])?;
        self.spc_or_lf();
        if !self.try_keyword("end") {
            return self.err("`end` expected for def");
        }
        let vargs_ids: Vec<SymbolId> = vargs.iter().map(|s| symbol::intern(s)).collect();
        let proc = Expression::Proc {
            vargs: vargs_ids,
            retval: Some(symbol::intern("return")),
            body,
        };
        Ok(Expression::Let {
            var: symbol::intern(&name),
            env: None,
            value: Box::new(proc),
        })
    }

    fn parse_expression(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_bool_and()?;
        loop {
            self.spc();
            let save = self.pos;
            if self.try_eat("||") {
                self.spc_or_lf();
                let rhs = self.parse_bool_and()?;
                lhs = make_call_method(lhs, "pipepipe", vec![rhs]);
            } else {
                self.pos = save;
                break;
            }
        }
        Ok(lhs)
    }

    fn parse_bool_and(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_comp()?;
        loop {
            self.spc();
            let save = self.pos;
            if self.try_eat("&&") {
                self.spc_or_lf();
                let rhs = self.parse_comp()?;
                lhs = make_call_method(lhs, "ampamp", vec![rhs]);
            } else {
                self.pos = save;
                break;
            }
        }
        Ok(lhs)
    }

    fn parse_comp(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_bit_or()?;
        loop {
            self.spc();
            let save = self.pos;
            // Order matters: longest first.
            let op = if self.try_eat("===") {
                Some("equal")
            } else if self.try_eat("==") {
                Some("equal")
            } else if self.try_eat("!=") {
                Some("notEqual")
            } else if self.try_eat("<=") {
                Some("lessOrEqual")
            } else if self.try_eat(">=") {
                Some("greaterOrEqual")
            } else if self.try_eat("<") {
                Some("lessThan")
            } else if self.try_eat(">") {
                Some("greaterThan")
            } else {
                None
            };
            match op {
                Some(name) => {
                    self.spc_or_lf();
                    let rhs = self.parse_bit_or()?;
                    lhs = make_call_method(lhs, name, vec![rhs]);
                }
                None => {
                    self.pos = save;
                    break;
                }
            }
        }
        Ok(lhs)
    }

    fn parse_bit_or(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_bit_and()?;
        loop {
            self.spc();
            let save = self.pos;
            if self.peek() == Some(b'|') && self.peek_at(1) != Some(b'|') {
                self.pos += 1;
                self.spc_or_lf();
                let rhs = self.parse_bit_and()?;
                lhs = make_call_method(lhs, "pipe", vec![rhs]);
            } else {
                self.pos = save;
                break;
            }
        }
        Ok(lhs)
    }

    fn parse_bit_and(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_bit_shift()?;
        loop {
            self.spc();
            let save = self.pos;
            if self.peek() == Some(b'&') && self.peek_at(1) != Some(b'&') {
                self.pos += 1;
                self.spc_or_lf();
                let rhs = self.parse_bit_shift()?;
                lhs = make_call_method(lhs, "amp", vec![rhs]);
            } else {
                self.pos = save;
                break;
            }
        }
        Ok(lhs)
    }

    fn parse_bit_shift(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_sim()?;
        loop {
            self.spc();
            let save = self.pos;
            let op = if self.try_eat("<<") {
                Some("ltlt")
            } else if self.try_eat(">>") {
                Some("gtgt")
            } else {
                None
            };
            match op {
                Some(name) => {
                    self.spc_or_lf();
                    let rhs = self.parse_sim()?;
                    lhs = make_call_method(lhs, name, vec![rhs]);
                }
                None => {
                    self.pos = save;
                    break;
                }
            }
        }
        Ok(lhs)
    }

    fn parse_sim(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_term()?;
        loop {
            self.spc();
            let save = self.pos;
            let op = if self.peek() == Some(b'+') {
                self.pos += 1;
                Some("plus")
            } else if self.peek() == Some(b'-') {
                self.pos += 1;
                Some("minus")
            } else {
                None
            };
            match op {
                Some(name) => {
                    self.spc_or_lf();
                    let rhs = self.parse_term()?;
                    lhs = make_call_method(lhs, name, vec![rhs]);
                }
                None => {
                    self.pos = save;
                    break;
                }
            }
        }
        Ok(lhs)
    }

    fn parse_term(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_factor()?;
        loop {
            self.spc();
            let save = self.pos;
            let op = if self.peek() == Some(b'*') {
                self.pos += 1;
                Some("asterisk")
            } else if self.peek() == Some(b'/') {
                self.pos += 1;
                Some("slash")
            } else if self.peek() == Some(b'%') {
                self.pos += 1;
                Some("percent")
            } else {
                None
            };
            match op {
                Some(name) => {
                    self.spc_or_lf();
                    let rhs = self.parse_factor()?;
                    lhs = make_call_method(lhs, name, vec![rhs]);
                }
                None => {
                    self.pos = save;
                    break;
                }
            }
        }
        Ok(lhs)
    }

    fn parse_factor(&mut self) -> Result<Expression, ParseError> {
        self.spc();
        if self.peek() == Some(b'!') && self.peek_at(1) != Some(b'=') {
            self.pos += 1;
            self.spc_or_lf();
            let f = self.parse_factor()?;
            return Ok(make_call_method(f, "exclamation", vec![]));
        }
        if self.peek() == Some(b'~') {
            self.pos += 1;
            self.spc_or_lf();
            let f = self.parse_factor()?;
            return Ok(make_call_method(f, "invert", vec![]));
        }
        self.parse_instance_method()
    }

    fn parse_instance_method(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_primary_chain()?;
        // Handle `.method(args)` or `.method` (no parens — bare slot reference).
        loop {
            self.spc();
            if self.peek() == Some(b'.') {
                let save = self.pos;
                self.pos += 1;
                self.spc_or_lf();
                match self.try_ident() {
                    Some(name) => {
                        // Optional `(args)`.
                        let save2 = self.pos;
                        if self.peek() == Some(b'(') {
                            self.pos += 1;
                            let args = self.parse_call_args()?;
                            lhs = Expression::Call {
                                receiver: Some(Box::new(lhs)),
                                method: symbol::intern(&name),
                                args,
                            };
                        } else {
                            self.pos = save2;
                            // Bare slot access: Ref { var: name, env: Some(lhs) }
                            lhs = Expression::Ref {
                                var: symbol::intern(&name),
                                env: Some(Box::new(lhs)),
                            };
                        }
                    }
                    None => {
                        self.pos = save;
                        break;
                    }
                }
            } else {
                break;
            }
        }
        Ok(lhs)
    }

    fn parse_primary_chain(&mut self) -> Result<Expression, ParseError> {
        let mut lhs = self.parse_primary()?;
        // `f(args)` after primary
        loop {
            self.spc();
            if self.peek() == Some(b'(') {
                self.pos += 1;
                let args = self.parse_call_args()?;
                lhs = match lhs {
                    Expression::Ref { var, env: None } => Expression::Call {
                        receiver: None,
                        method: var,
                        args,
                    },
                    other => Expression::Call {
                        receiver: Some(Box::new(other)),
                        method: symbol::intern("call"),
                        args,
                    },
                };
                continue;
            }
            // Bareword call: `f arg1, arg2` (no parens).
            if matches!(lhs, Expression::Ref { var: _, env: None }) && self.bareword_arg_allowed() {
                let mut args = vec![self.parse_expression()?];
                loop {
                    self.spc();
                    if self.peek() == Some(b',') {
                        self.pos += 1;
                        self.spc_or_lf();
                        args.push(self.parse_expression()?);
                    } else {
                        break;
                    }
                }
                lhs = match lhs {
                    Expression::Ref { var, env: None } => Expression::Call {
                        receiver: None,
                        method: var,
                        args,
                    },
                    _ => unreachable!(),
                };
                continue;
            }
            break;
        }
        Ok(lhs)
    }

    /// Whether the next non-space char can start a bareword argument. The
    /// stop set mirrors csru's `~[` / `~.` / `~if` / ... negated lookaheads.
    fn bareword_arg_allowed(&self) -> bool {
        // Skip horizontal whitespace (we're inside parse_primary_chain after a `.spc()`).
        let mut p = self.pos;
        while let Some(&c) = self.src.get(p) {
            if c == b' ' || c == b'\t' {
                p += 1;
            } else {
                break;
            }
        }
        let c = match self.src.get(p) {
            Some(&c) => c,
            None => return false,
        };
        if matches!(
            c,
            b'\n' | b'\r' | b';' | b'[' | b'.' | b'|' | b'&' | b'=' | b'!'
                | b'>' | b'<' | b'+' | b'-' | b'*' | b'/' | b'%' | b'~' | b'#'
                | b')' | b']' | b'}' | b',' | b':'
        ) {
            return false;
        }
        // Block-end / chain keywords.
        for kw in &["end", "else", "elsif", "then", "if", "while", "def", "class"] {
            let bytes = kw.as_bytes();
            if self.src[p..].starts_with(bytes) {
                let after = self.src.get(p + bytes.len()).copied();
                if !matches!(after, Some(b) if is_ident_cont(b)) {
                    return false;
                }
            }
        }
        true
    }

    fn parse_call_args(&mut self) -> Result<Vec<Expression>, ParseError> {
        let mut args = Vec::new();
        self.spc_or_lf();
        if self.try_eat(")") {
            return Ok(args);
        }
        loop {
            self.spc_or_lf();
            let a = self.parse_expression()?;
            args.push(a);
            self.spc_or_lf();
            if self.try_eat(",") {
                continue;
            }
            self.spc_or_lf();
            if self.try_eat(")") {
                return Ok(args);
            }
            return self.err("expected `,` or `)`");
        }
    }

    fn parse_primary(&mut self) -> Result<Expression, ParseError> {
        self.spc();
        match self.peek() {
            Some(b'(') => {
                self.pos += 1;
                self.spc_or_lf();
                let e = self.parse_statement()?;
                self.spc_or_lf();
                if !self.try_eat(")") {
                    return self.err("`)` expected");
                }
                Ok(e)
            }
            Some(b'[') => self.parse_array_literal(),
            Some(b'{') => self.parse_closure_literal(),
            Some(b'"') => self.parse_string_literal(),
            Some(c) if c.is_ascii_digit() => self.parse_number_literal(),
            _ => {
                // Identifier.
                match self.try_ident() {
                    Some(name) => Ok(Expression::Ref {
                        var: symbol::intern(&name),
                        env: None,
                    }),
                    None => self.err("expected expression"),
                }
            }
        }
    }

    fn parse_number_literal(&mut self) -> Result<Expression, ParseError> {
        // Scan [0-9.]+. If contains '.', it's real; else int. Trailing '.' is rejected.
        let start = self.pos;
        let mut has_dot = false;
        while let Some(c) = self.peek() {
            if c.is_ascii_digit() {
                self.pos += 1;
            } else if c == b'.' && !has_dot {
                // Only consume '.' if followed by a digit.
                if let Some(c2) = self.peek_at(1) {
                    if c2.is_ascii_digit() {
                        has_dot = true;
                        self.pos += 1;
                        continue;
                    }
                }
                break;
            } else {
                break;
            }
        }
        let s = std::str::from_utf8(&self.src[start..self.pos])
            .unwrap()
            .to_string();
        // Desugar: Numeric.parse("123")
        Ok(Expression::Call {
            receiver: Some(Box::new(ref_expr("Numeric"))),
            method: symbol::intern("parse"),
            args: vec![Expression::StrLit(s)],
        })
    }

    fn parse_string_literal(&mut self) -> Result<Expression, ParseError> {
        self.pos += 1; // opening "
        let mut buf = String::new();
        loop {
            match self.peek() {
                None => return self.err("unterminated string literal"),
                Some(b'"') => {
                    self.pos += 1;
                    return Ok(Expression::StrLit(buf));
                }
                Some(b'\\') => {
                    self.pos += 1;
                    match self.peek() {
                        Some(b'n') => buf.push('\n'),
                        Some(b'r') => buf.push('\r'),
                        Some(b'\\') => buf.push('\\'),
                        Some(b'"') => buf.push('"'),
                        Some(c) => buf.push(c as char),
                        None => return self.err("unterminated escape"),
                    }
                    self.pos += 1;
                }
                Some(c) => {
                    buf.push(c as char);
                    self.pos += 1;
                }
            }
        }
    }

    fn parse_array_literal(&mut self) -> Result<Expression, ParseError> {
        self.pos += 1; // [
        let mut items: Vec<Expression> = Vec::new();
        self.spc_or_lf();
        if self.try_eat("]") {
            // Empty array.
            return Ok(make_array_call(items));
        }
        loop {
            self.spc_or_lf();
            let e = self.parse_expression()?;
            items.push(e);
            self.spc_or_lf();
            if self.try_eat(",") {
                continue;
            }
            self.spc_or_lf();
            if self.try_eat("]") {
                return Ok(make_array_call(items));
            }
            return self.err("expected `,` or `]`");
        }
    }

    fn parse_closure_literal(&mut self) -> Result<Expression, ParseError> {
        self.pos += 1; // {
        // Optional varg list: | a, b [: ret] |
        let mut vargs: Vec<String> = Vec::new();
        let mut retval: Option<String> = None;
        self.spc_or_lf();
        if self.peek() == Some(b'|') {
            self.pos += 1;
            self.spc_or_lf();
            if !self.try_eat("|") {
                if !(self.peek() == Some(b':')) {
                    loop {
                        self.spc_or_lf();
                        let id = self
                            .try_ident()
                            .ok_or_else(|| self.make_err("varg name expected"))?;
                        vargs.push(id);
                        self.spc_or_lf();
                        if self.try_eat(",") {
                            continue;
                        }
                        break;
                    }
                }
                self.spc_or_lf();
                if self.try_eat(":") {
                    self.spc_or_lf();
                    retval = Some(
                        self.try_ident()
                            .ok_or_else(|| self.make_err("retval name expected"))?,
                    );
                }
                self.spc_or_lf();
                if !self.try_eat("|") {
                    return self.err("`|` expected to close varg list");
                }
            }
        }
        let body = self.parse_statements(&["}"])?;
        self.spc_or_lf();
        if !self.try_eat("}") {
            return self.err("`}` expected");
        }
        let vargs_ids: Vec<SymbolId> = vargs.iter().map(|s| symbol::intern(s)).collect();
        Ok(Expression::Proc {
            vargs: vargs_ids,
            retval: retval.map(|s| symbol::intern(&s)),
            body,
        })
    }

    fn make_err(&self, msg: &str) -> ParseError {
        ParseError {
            msg: msg.to_string(),
            pos: self.pos,
        }
    }
}

fn is_ident_start(c: u8) -> bool {
    c.is_ascii_alphabetic() || c == b'_'
}

fn is_ident_cont(c: u8) -> bool {
    c.is_ascii_alphanumeric() || c == b'_' || c == b'!'
}

fn ref_expr(name: &str) -> Expression {
    Expression::Ref {
        var: symbol::intern(name),
        env: None,
    }
}

fn make_call_method(recv: Expression, method: &str, args: Vec<Expression>) -> Expression {
    Expression::Call {
        receiver: Some(Box::new(recv)),
        method: symbol::intern(method),
        args,
    }
}

fn make_proc(vargs: Vec<SymbolId>, retval: Option<SymbolId>, body: Vec<Expression>) -> Expression {
    Expression::Proc { vargs, retval, body }
}

fn make_array_call(items: Vec<Expression>) -> Expression {
    // Desugar [a, b, c] -> Array.new().push(a, b, c)
    let new_call = Expression::Call {
        receiver: Some(Box::new(ref_expr("Array"))),
        method: symbol::intern("new"),
        args: vec![],
    };
    if items.is_empty() {
        return new_call;
    }
    Expression::Call {
        receiver: Some(Box::new(new_call)),
        method: symbol::intern("push"),
        args: items,
    }
}

/// `lhs = rhs` desugaring. lhs is the LHS expression we parsed; we have to
/// re-cast it to a LetExpression. Supported forms:
///   - `Ref { var, env: None }` -> Let { var, env: None, value }
///   - `Ref { var, env: Some(e) }` -> Let { var, env: Some(e), value }
fn make_let_from(lhs: Expression, rhs: Expression) -> Expression {
    match lhs {
        Expression::Ref { var, env } => Expression::Let {
            var,
            env,
            value: Box::new(rhs),
        },
        // Other LHS forms (e.g., a[i] = v) — unsupported for now.
        _ => Expression::Let {
            var: symbol::intern("_invalid_let"),
            env: None,
            value: Box::new(rhs),
        },
    }
}

/// Desugar `while (cond) body end` into a recursive named closure.
/// We need a unique name per loop to avoid clobbering. Use a counter.
static WHILE_COUNTER: std::sync::atomic::AtomicUsize = std::sync::atomic::AtomicUsize::new(0);

fn make_while_desugar(cond: Expression, body: Vec<Expression>) -> Expression {
    let n = WHILE_COUNTER.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
    let loop_name = format!("__while_{}", n);
    let loop_sym = symbol::intern(&loop_name);

    // body_with_recurse = body; __while_N()
    let recurse = Expression::Call {
        receiver: Some(Box::new(Expression::Ref {
            var: loop_sym,
            env: None,
        })),
        method: symbol::intern("call"),
        args: vec![],
    };
    let mut body_with_recurse = body;
    body_with_recurse.push(recurse);

    // if (cond) body_with_recurse end
    let if_expr = make_call_method(
        cond,
        "ifTrue",
        vec![make_proc(vec![], None, body_with_recurse)],
    );

    // __while_N = { if_expr }
    let loop_proc = make_proc(vec![], None, vec![if_expr]);
    let assign = Expression::Let {
        var: loop_sym,
        env: None,
        value: Box::new(loop_proc),
    };

    // __while_N()
    let call_loop = Expression::Call {
        receiver: Some(Box::new(Expression::Ref {
            var: loop_sym,
            env: None,
        })),
        method: symbol::intern("call"),
        args: vec![],
    };

    // Wrap as immediate-call closure so the assignment is local.
    let wrap = make_proc(vec![], None, vec![assign, call_loop]);
    Expression::Call {
        receiver: Some(Box::new(wrap)),
        method: symbol::intern("call"),
        args: vec![],
    }
}
