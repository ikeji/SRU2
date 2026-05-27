use std::collections::HashMap;
use std::sync::{Mutex, OnceLock};

pub type SymbolId = u32;

struct SymbolTable {
    forward: HashMap<String, SymbolId>,
    reverse: Vec<String>,
}

static TABLE: OnceLock<Mutex<SymbolTable>> = OnceLock::new();

fn table() -> &'static Mutex<SymbolTable> {
    TABLE.get_or_init(|| {
        Mutex::new(SymbolTable {
            forward: HashMap::new(),
            reverse: Vec::new(),
        })
    })
}

pub fn intern(name: &str) -> SymbolId {
    let mut t = table().lock().unwrap();
    if let Some(&id) = t.forward.get(name) {
        return id;
    }
    let id = t.reverse.len() as SymbolId;
    t.reverse.push(name.to_string());
    t.forward.insert(name.to_string(), id);
    id
}

pub fn name(id: SymbolId) -> String {
    table().lock().unwrap().reverse[id as usize].clone()
}
