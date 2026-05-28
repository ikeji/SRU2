//! `rsruc input.sru -o output.bc` — compile SRU source to bytecode.

use std::fs;
use std::path::PathBuf;
use std::process::ExitCode;

use rsru::bytecode;
use rsru::parser;

fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().collect();
    let mut input: Option<PathBuf> = None;
    let mut output: Option<PathBuf> = None;
    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "-o" => {
                if i + 1 >= args.len() {
                    eprintln!("rsruc: -o requires an argument");
                    return ExitCode::from(2);
                }
                output = Some(args[i + 1].clone().into());
                i += 2;
            }
            other => {
                if input.is_some() {
                    eprintln!("rsruc: multiple inputs not supported");
                    return ExitCode::from(2);
                }
                input = Some(other.into());
                i += 1;
            }
        }
    }
    let input = match input {
        Some(p) => p,
        None => {
            eprintln!("usage: rsruc <input.sru> -o <output.bc>");
            return ExitCode::from(2);
        }
    };
    let output = output.unwrap_or_else(|| input.with_extension("bc"));

    let src = match fs::read_to_string(&input) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("rsruc: read {}: {}", input.display(), e);
            return ExitCode::from(1);
        }
    };

    let exprs = match parser::parse(&src) {
        Ok(e) => e,
        Err(e) => {
            eprintln!("rsruc: {}", e.render(&src));
            return ExitCode::from(1);
        }
    };

    let module = bytecode::compile::compile(&exprs);
    let bytes = bytecode::encode::encode(&module);
    if let Err(e) = fs::write(&output, &bytes) {
        eprintln!("rsruc: write {}: {}", output.display(), e);
        return ExitCode::from(1);
    }
    eprintln!(
        "rsruc: wrote {} ({} bytes, {} strings, {} symbols, {} procs)",
        output.display(),
        bytes.len(),
        module.strings.len(),
        module.symbols.len(),
        module.procs.len()
    );
    ExitCode::SUCCESS
}
