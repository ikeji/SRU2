use crate::object::{BasicObject, ObjData, ObjId};
use crate::symbol::SymbolId;

/// Simple bump-allocated heap. No GC for now — we leak. Step A scope only.
pub struct Heap {
    slots: Vec<BasicObject>,
}

impl Heap {
    pub fn new() -> Self {
        Self { slots: Vec::new() }
    }

    pub fn alloc(&mut self, obj: BasicObject) -> ObjId {
        let id = ObjId(self.slots.len() as u32);
        self.slots.push(obj);
        id
    }

    pub fn alloc_with_data(&mut self, data: ObjData) -> ObjId {
        self.alloc(BasicObject::with_data(data))
    }

    pub fn alloc_empty(&mut self) -> ObjId {
        self.alloc(BasicObject::empty())
    }

    #[inline]
    pub fn get(&self, id: ObjId) -> &BasicObject {
        &self.slots[id.0 as usize]
    }

    #[inline]
    pub fn get_mut(&mut self, id: ObjId) -> &mut BasicObject {
        &mut self.slots[id.0 as usize]
    }

    pub fn set_slot(&mut self, id: ObjId, name: SymbolId, value: ObjId) {
        self.get_mut(id).fields.insert(name, value);
    }

    pub fn get_slot(&self, id: ObjId, name: SymbolId) -> Option<ObjId> {
        self.get(id).fields.get(&name).copied()
    }

    pub fn has_slot(&self, id: ObjId, name: SymbolId) -> bool {
        self.get(id).fields.contains_key(&name)
    }
}
