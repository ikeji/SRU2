//! Tiny binary serialiser for `Module`. Format is little-endian.
//!
//! Layout (version 2):
//!   4   magic     b"RSBC"
//!   4   version   u32 LE
//!   strings: u32 count + N×(u32 len + utf8 bytes)
//!   symbols: u32 count + N×(u32 len + utf8 bytes)
//!   procs:   u32 count + N×ProcDef
//!             u32 vargs_len + N×u32 indices
//!             u8 has_retval + (u32 retval if present)
//!             insn list
//!   top:     insn list
//!   source:  u32 len + utf8 bytes  (NEW in v2)
//!
//! `Insn` encoding: u32 pos, then 1-byte tag + payload.
//!   0x01 Lds u32
//!   0x02 Ref u32 + u8(has_env)
//!   0x03 Let u32 + u8(has_env)
//!   0x04 Call u32 method + u32 argc + u8(has_recv)
//!   0x05 Proc u32
//!   0x06 Pop
//!
//! Version 1 (no per-insn pos, no source) is still readable; missing fields
//! default to `WIRE_POS_UNKNOWN` / empty source.

use super::{Insn, InsnKind, Module, ProcDef, WIRE_POS_UNKNOWN};

const MAGIC: &[u8; 4] = b"RSBC";
const VERSION: u32 = 2;

pub fn encode(m: &Module) -> Vec<u8> {
    let mut out = Vec::with_capacity(1024);
    out.extend_from_slice(MAGIC);
    write_u32(&mut out, VERSION);
    write_strings(&mut out, &m.strings);
    write_strings(&mut out, &m.symbols);
    write_u32(&mut out, m.procs.len() as u32);
    for p in &m.procs {
        write_proc(&mut out, p);
    }
    write_insns(&mut out, &m.top);
    // v2 trailer: source text.
    let src_bytes = m.source.as_bytes();
    write_u32(&mut out, src_bytes.len() as u32);
    out.extend_from_slice(src_bytes);
    out
}

pub fn decode(bytes: &[u8]) -> Result<Module, String> {
    let mut r = Reader { bytes, pos: 0 };
    let magic = r.read_bytes(4)?;
    if magic != MAGIC {
        return Err("bad magic".into());
    }
    let v = r.read_u32()?;
    let has_pos_and_source = match v {
        1 => false,
        2 => true,
        other => return Err(format!("unsupported version {}", other)),
    };
    let strings = r.read_strings()?;
    let symbols = r.read_strings()?;
    let proc_count = r.read_u32()? as usize;
    let mut procs = Vec::with_capacity(proc_count);
    for _ in 0..proc_count {
        procs.push(r.read_proc(has_pos_and_source)?);
    }
    let top = r.read_insns(has_pos_and_source)?;
    let source = if has_pos_and_source {
        let len = r.read_u32()? as usize;
        let bytes = r.read_bytes(len)?;
        std::str::from_utf8(bytes)
            .map_err(|e| format!("bad utf8 source: {}", e))?
            .to_string()
    } else {
        String::new()
    };
    Ok(Module { strings, symbols, procs, top, source })
}

fn write_u32(out: &mut Vec<u8>, v: u32) {
    out.extend_from_slice(&v.to_le_bytes());
}

fn write_strings(out: &mut Vec<u8>, ss: &[String]) {
    write_u32(out, ss.len() as u32);
    for s in ss {
        let bytes = s.as_bytes();
        write_u32(out, bytes.len() as u32);
        out.extend_from_slice(bytes);
    }
}

fn write_proc(out: &mut Vec<u8>, p: &ProcDef) {
    write_u32(out, p.vargs.len() as u32);
    for v in &p.vargs {
        write_u32(out, *v);
    }
    match p.retval {
        Some(r) => {
            out.push(1);
            write_u32(out, r);
        }
        None => out.push(0),
    }
    write_insns(out, &p.body);
}

fn write_insns(out: &mut Vec<u8>, insns: &[Insn]) {
    write_u32(out, insns.len() as u32);
    for i in insns {
        write_u32(out, i.pos);
        match &i.kind {
            InsnKind::Lds(k) => {
                out.push(0x01);
                write_u32(out, *k);
            }
            InsnKind::Ref { var, has_env } => {
                out.push(0x02);
                write_u32(out, *var);
                out.push(if *has_env { 1 } else { 0 });
            }
            InsnKind::Let { var, has_env } => {
                out.push(0x03);
                write_u32(out, *var);
                out.push(if *has_env { 1 } else { 0 });
            }
            InsnKind::Call {
                method,
                argc,
                has_recv,
            } => {
                out.push(0x04);
                write_u32(out, *method);
                write_u32(out, *argc);
                out.push(if *has_recv { 1 } else { 0 });
            }
            InsnKind::Proc(k) => {
                out.push(0x05);
                write_u32(out, *k);
            }
            InsnKind::Pop => out.push(0x06),
        }
    }
}

struct Reader<'a> {
    bytes: &'a [u8],
    pos: usize,
}

impl<'a> Reader<'a> {
    fn read_bytes(&mut self, n: usize) -> Result<&'a [u8], String> {
        if self.pos + n > self.bytes.len() {
            return Err("unexpected EOF".into());
        }
        let s = &self.bytes[self.pos..self.pos + n];
        self.pos += n;
        Ok(s)
    }
    fn read_u8(&mut self) -> Result<u8, String> {
        let b = self.read_bytes(1)?;
        Ok(b[0])
    }
    fn read_u32(&mut self) -> Result<u32, String> {
        let b = self.read_bytes(4)?;
        Ok(u32::from_le_bytes([b[0], b[1], b[2], b[3]]))
    }
    fn read_strings(&mut self) -> Result<Vec<String>, String> {
        let n = self.read_u32()? as usize;
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            let len = self.read_u32()? as usize;
            let bytes = self.read_bytes(len)?;
            out.push(
                std::str::from_utf8(bytes)
                    .map_err(|e| format!("bad utf8: {}", e))?
                    .to_string(),
            );
        }
        Ok(out)
    }
    fn read_proc(&mut self, with_pos: bool) -> Result<ProcDef, String> {
        let vn = self.read_u32()? as usize;
        let mut vargs = Vec::with_capacity(vn);
        for _ in 0..vn {
            vargs.push(self.read_u32()?);
        }
        let has_retval = self.read_u8()?;
        let retval = if has_retval != 0 {
            Some(self.read_u32()?)
        } else {
            None
        };
        let body = self.read_insns(with_pos)?;
        Ok(ProcDef { vargs, retval, body })
    }
    fn read_insns(&mut self, with_pos: bool) -> Result<Vec<Insn>, String> {
        let n = self.read_u32()? as usize;
        let mut out = Vec::with_capacity(n);
        for _ in 0..n {
            let pos = if with_pos { self.read_u32()? } else { WIRE_POS_UNKNOWN };
            let tag = self.read_u8()?;
            let kind = match tag {
                0x01 => InsnKind::Lds(self.read_u32()?),
                0x02 => InsnKind::Ref {
                    var: self.read_u32()?,
                    has_env: self.read_u8()? != 0,
                },
                0x03 => InsnKind::Let {
                    var: self.read_u32()?,
                    has_env: self.read_u8()? != 0,
                },
                0x04 => InsnKind::Call {
                    method: self.read_u32()?,
                    argc: self.read_u32()?,
                    has_recv: self.read_u8()? != 0,
                },
                0x05 => InsnKind::Proc(self.read_u32()?),
                0x06 => InsnKind::Pop,
                _ => return Err(format!("bad insn tag {:#x}", tag)),
            };
            out.push(Insn { kind, pos });
        }
        Ok(out)
    }
}
