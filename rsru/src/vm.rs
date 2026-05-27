use crate::eval::StackFrame;
use crate::heap::Heap;
use crate::object::ObjId;
use crate::symbol;

pub struct Vm {
    pub heap: Heap,
    pub current_frame: StackFrame,
    pub root_binding: ObjId,
    pub builtin: BuiltinRefs,
}

/// Well-known object ids set up at boot.
#[derive(Default)]
pub struct BuiltinRefs {
    pub nil_id: ObjId,
    pub true_id: ObjId,
    pub false_id: ObjId,
    pub class_cls: ObjId,
    pub object_cls: ObjId,
    pub nil_cls: ObjId,
    pub boolean_cls: ObjId,
    pub numeric_cls: ObjId,
    pub string_cls: ObjId,
    pub array_cls: ObjId,
    pub proc_cls: ObjId,
    pub binding_cls: ObjId,
    pub math_cls: ObjId,
    pub sys_cls: ObjId,
    /// The `__parser` object exposed in the root binding.
    pub parser_id: ObjId,
}

const PRELUDE: &str = include_str!("prelude.sru");

fn load_prelude(vm: &mut Vm) {
    vm.eval_source(PRELUDE);
}

impl Vm {
    pub fn new() -> Self {
        let mut heap = Heap::new();
        let placeholder = heap.alloc_empty();
        let root_binding = placeholder;
        let mut vm = Self {
            heap,
            current_frame: StackFrame::new(root_binding, Vec::new()),
            root_binding,
            builtin: BuiltinRefs::default(),
        };
        crate::builtin::bootstrap(&mut vm);
        load_prelude(&mut vm);
        vm
    }

    #[inline]
    pub fn frame(&self) -> &StackFrame {
        &self.current_frame
    }

    #[inline]
    pub fn frame_mut(&mut self) -> &mut StackFrame {
        &mut self.current_frame
    }

    /// Run a string of SRU source in the root binding. Used for prelude load.
    pub fn eval_source(&mut self, src: &str) {
        let mut pos = 0;
        loop {
            match crate::parser::parse_one_statement(Some(self), src, pos) {
                Ok((new_pos, Some(expr))) => {
                    let _ = crate::eval::eval_program(self, vec![expr]);
                    pos = new_pos;
                }
                Ok((_, None)) => break,
                Err(e) => panic!("prelude parse error at {}: {}", e.pos, e.msg),
            }
        }
    }

    /// Create a Binding object parented at `parent`.
    pub fn make_binding(&mut self, parent: ObjId) -> ObjId {
        let id = self.heap.alloc_empty();
        let klass_sym = symbol::intern("klass");
        let parent_sym = symbol::intern("_parent");
        self.heap.set_slot(id, klass_sym, self.builtin.binding_cls);
        if parent != self.builtin.nil_id {
            self.heap.set_slot(id, parent_sym, parent);
        }
        id
    }
}
