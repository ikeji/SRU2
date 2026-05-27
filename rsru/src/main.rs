use std::env;
use std::fs;
use std::io::{self, Read};

use rsru::eval;
use rsru::parser;
use rsru::Vm;

fn main() {
    let args: Vec<String> = env::args().collect();
    let src = if args.len() > 1 {
        fs::read_to_string(&args[1]).expect("failed to read file")
    } else {
        let mut s = String::new();
        io::stdin().read_to_string(&mut s).expect("read stdin");
        s
    };

    let mut vm = Vm::new();
    let mut pos = 0;
    loop {
        let (new_pos, expr) = match parser::parse_one_statement(Some(&mut vm), &src, pos) {
            Ok(r) => r,
            Err(e) => {
                eprintln!("Parse error at byte {}: {}", e.pos, e.msg);
                std::process::exit(1);
            }
        };
        match expr {
            Some(e) => {
                let _ = eval::eval_program(&mut vm, vec![e]);
                pos = new_pos;
            }
            None => break,
        }
    }
}
