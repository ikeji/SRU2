use crate::builtin::{as_str, nil};
use crate::object::{NumVal, ObjData, ObjId, ProcKind};
use crate::symbol;
use crate::vm::Vm;

pub fn install(vm: &mut Vm, root_binding: ObjId) {
    bind_global(vm, root_binding, "p", p);
    bind_global(vm, root_binding, "puts", puts);
    bind_global(vm, root_binding, "print", print);
    bind_global(vm, root_binding, "exit", exit);
}

fn bind_global(vm: &mut Vm, bind: ObjId, name: &str, f: crate::object::NativeFn) {
    let id = vm.heap.alloc_with_data(ObjData::Proc(ProcKind::Native(f)));
    vm.heap.set_slot(bind, symbol::intern(name), id);
}

fn p(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    // args[0] = binding (since `p` is called without receiver).
    let v = args.get(1).copied().unwrap_or(vm.builtin.nil_id);
    println!("{}", inspect(vm, v));
    nil(vm)
}

fn puts(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let v = args.get(1).copied().unwrap_or(vm.builtin.nil_id);
    // csru's puts is String-only; non-String prints empty line.
    let s = as_str(vm, v).unwrap_or_default();
    println!("{}", s);
    nil(vm)
}

fn print(vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let v = args.get(1).copied().unwrap_or(vm.builtin.nil_id);
    let s = as_str(vm, v).unwrap_or_default();
    print!("{}", s);
    nil(vm)
}

fn exit(_vm: &mut Vm, args: &[ObjId]) -> ObjId {
    let _ = args;
    std::process::exit(0)
}

pub fn inspect(vm: &Vm, id: ObjId) -> String {
    if id == vm.builtin.nil_id {
        return "<nil class:<Object class:...>>".to_string();
    }
    if id == vm.builtin.true_id {
        return "<True ampamp:... class:... equal:... exclamation:... ifFalse:... ifTrue:... ifTrueFalse:... notEqual:... pipepipe:...>".to_string();
    }
    if id == vm.builtin.false_id {
        return "<False ampamp:... class:... equal:... exclamation:... ifFalse:... ifTrue:... ifTrueFalse:... notEqual:... pipepipe:...>".to_string();
    }
    match &vm.heap.get(id).data {
        Some(ObjData::Num(NumVal::Int(i))) => format!("<Numeric({})>", i),
        Some(ObjData::Num(NumVal::Real(f))) => format!("<Numeric({})>", f),
        Some(ObjData::Str(s)) => format!("<String({:?})>", s),
        Some(ObjData::Array(v)) => {
            let inner: Vec<String> = v.iter().map(|x| inspect(vm, *x)).collect();
            format!("<Array size={} [{}]>", v.len(), inner.join(", "))
        }
        Some(ObjData::Proc(_)) => "<Proc(...)>".to_string(),
        Some(ObjData::Frame(_)) => "<Frame>".to_string(),
        None => "<Object>".to_string(),
    }
}
