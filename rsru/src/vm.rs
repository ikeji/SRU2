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
}

impl Vm {
    pub fn new() -> Self {
        let mut heap = Heap::new();
        // Sentinel for "uninitialized" ObjId — slot 0 is the eventual `nil`.
        // We'll allocate nil first below.
        let placeholder = heap.alloc_empty();
        let root_binding = placeholder; // will replace
        let mut vm = Self {
            heap,
            current_frame: StackFrame::new(root_binding, Vec::new()),
            root_binding,
            builtin: BuiltinRefs::default(),
        };
        crate::builtin::bootstrap(&mut vm);
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
