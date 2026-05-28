use std::env;
use std::fs;
use std::io::{self, BufRead, IsTerminal, Read, Write};

use rsru::builtin::io as bio;
use rsru::eval;
use rsru::parser;
use rsru::Vm;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() > 1 {
        let path = &args[1];
        let bytes = fs::read(path).expect("failed to read file");
        // Detect bytecode by magic; otherwise treat as SRU source.
        if bytes.len() >= 4 && &bytes[0..4] == b"RSBC" {
            run_bytecode(&bytes);
        } else {
            let src = String::from_utf8(bytes).expect("file not utf8");
            run_source(&src);
        }
    } else if io::stdin().is_terminal() {
        run_repl();
    } else {
        let mut s = String::new();
        io::stdin().read_to_string(&mut s).expect("read stdin");
        run_source(&s);
    }
}

fn run_bytecode(bytes: &[u8]) {
    let module = match rsru::bytecode::encode::decode(bytes) {
        Ok(m) => m,
        Err(e) => {
            eprintln!("Bytecode load error: {}", e);
            std::process::exit(1);
        }
    };
    let exprs = rsru::bytecode::decode::to_program(&module);
    let mut vm = Vm::new();
    vm.source = module.source;
    let _ = eval::eval_program(&mut vm, exprs);
}

fn run_source(src: &str) {
    let mut vm = Vm::new();
    vm.source = src.to_string();
    let mut pos = 0;
    loop {
        let (new_pos, expr) = match parser::parse_one_statement(Some(&mut vm), src, pos) {
            Ok(r) => r,
            Err(e) => {
                eprintln!("{}", e.render(src));
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

fn run_repl() {
    let mut vm = Vm::new();
    let stdin = io::stdin();
    let mut stdout = io::stdout();
    let mut counter = 0usize;
    let mut input = String::new();
    let mut buf = String::new();
    println!("rsru REPL — empty line exits.");
    loop {
        // Carry over any unparsed leftover from the previous iteration.
        if input.is_empty() {
            print!(">> ");
        } else {
            print!(".. ");
        }
        stdout.flush().ok();
        buf.clear();
        match stdin.lock().read_line(&mut buf) {
            Ok(0) => {
                println!();
                break;
            }
            Ok(_) => {}
            Err(e) => {
                eprintln!("read error: {}", e);
                break;
            }
        }
        if buf.trim().is_empty() && input.is_empty() {
            break;
        }
        input.push_str(&buf);

        // Parse and eval one statement at a time, keeping leftover input
        // for multi-line entries.
        let mut pos = 0;
        loop {
            match parser::parse_one_statement(Some(&mut vm), &input, pos) {
                Ok((new_pos, Some(expr))) => {
                    let result = eval::eval_program(&mut vm, vec![expr]);
                    counter += 1;
                    let nil = vm.builtin.nil_id;
                    if result != nil {
                        let name = format!("_{}", counter);
                        let sym = rsru::symbol::intern(&name);
                        vm.heap.set_slot(vm.root_binding, sym, result);
                        vm.heap
                            .set_slot(vm.root_binding, rsru::symbol::intern("_"), result);
                        println!("{} = {}", name, bio::inspect(&vm, result));
                    }
                    pos = new_pos;
                }
                Ok((new_pos, None)) => {
                    if new_pos >= input.len() {
                        input.clear();
                    } else {
                        input.drain(..new_pos);
                    }
                    break;
                }
                Err(e) => {
                    // If the parse error happens at end-of-input, treat it
                    // as "need more lines" rather than an error.
                    let trimmed = input.trim_end();
                    if e.pos >= trimmed.len() {
                        break;
                    }
                    eprintln!("{}", e.render(&input));
                    input.clear();
                    break;
                }
            }
        }
    }
}
