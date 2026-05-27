use std::env;
use std::fs;
use std::io::{self, Read};

use rsru::Vm;
use rsru::parser;
use rsru::eval;

fn main() {
    let args: Vec<String> = env::args().collect();
    let src = if args.len() > 1 {
        fs::read_to_string(&args[1]).expect("failed to read file")
    } else {
        let mut s = String::new();
        io::stdin().read_to_string(&mut s).expect("read stdin");
        s
    };

    let exprs = match parser::parse(&src) {
        Ok(e) => e,
        Err(e) => {
            eprintln!("Parse error at byte {}: {}", e.pos, e.msg);
            std::process::exit(1);
        }
    };

    let mut vm = Vm::new();
    let _ = eval::eval_program(&mut vm, exprs);
}
