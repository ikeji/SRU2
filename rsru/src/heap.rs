use crate::object::{BasicObject, ObjData, ObjId};
use crate::symbol::SymbolId;

/// Heap of `BasicObject`s, indexed by stable `ObjId`. Supports mark-and-sweep
/// GC: dead slots are recycled via a free list, marks live outside the
/// objects themselves.
pub struct Heap {
    slots: Vec<HeapSlot>,
    /// Indices of `Free` slots, ready for reuse.
    free: Vec<u32>,
    /// Marks parallel to `slots`. true ⇒ reachable in the current GC cycle.
    marks: Vec<bool>,
    /// Allocations since last GC.
    alloc_count: usize,
    /// When `alloc_count` reaches this, the next safe point triggers a GC.
    next_gc_threshold: usize,
    /// Total objects ever allocated (for stats / debugging).
    pub stats_total_allocated: u64,
    /// Total objects ever freed.
    pub stats_total_freed: u64,
}

pub enum HeapSlot {
    Live(BasicObject),
    Free,
}

impl Heap {
    pub fn new() -> Self {
        Self {
            slots: Vec::new(),
            free: Vec::new(),
            marks: Vec::new(),
            alloc_count: 0,
            // Start GC after the first 4096 allocations. Bootstrap allocates
            // ~150 objects, so the first cycle happens well after boot.
            next_gc_threshold: 4096,
            stats_total_allocated: 0,
            stats_total_freed: 0,
        }
    }

    pub fn alloc(&mut self, obj: BasicObject) -> ObjId {
        self.alloc_count += 1;
        self.stats_total_allocated += 1;
        if let Some(i) = self.free.pop() {
            self.slots[i as usize] = HeapSlot::Live(obj);
            self.marks[i as usize] = false;
            return ObjId(i);
        }
        let id = ObjId(self.slots.len() as u32);
        self.slots.push(HeapSlot::Live(obj));
        self.marks.push(false);
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
        match &self.slots[id.0 as usize] {
            HeapSlot::Live(b) => b,
            HeapSlot::Free => panic!("access to freed slot {:?}", id),
        }
    }

    #[inline]
    pub fn get_mut(&mut self, id: ObjId) -> &mut BasicObject {
        match &mut self.slots[id.0 as usize] {
            HeapSlot::Live(b) => b,
            HeapSlot::Free => panic!("access to freed slot {:?}", id),
        }
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

    pub fn should_gc(&self) -> bool {
        self.alloc_count >= self.next_gc_threshold
    }

    /// Mark `id` and return true if this is the first time it's marked
    /// (so the caller knows to push its children onto the mark stack).
    pub fn mark(&mut self, id: ObjId) -> bool {
        let i = id.0 as usize;
        if self.marks[i] {
            return false;
        }
        self.marks[i] = true;
        true
    }

    pub fn is_marked(&self, id: ObjId) -> bool {
        self.marks[id.0 as usize]
    }

    /// Free unmarked slots and reset marks for the next cycle. Sets the next
    /// GC threshold so we don't thrash on small heaps but also don't let a
    /// large heap go unchecked.
    pub fn sweep(&mut self) {
        let mut live = 0usize;
        for i in 0..self.slots.len() {
            if self.marks[i] {
                live += 1;
            } else if matches!(self.slots[i], HeapSlot::Live(_)) {
                self.slots[i] = HeapSlot::Free;
                self.free.push(i as u32);
                self.stats_total_freed += 1;
            }
            self.marks[i] = false;
        }
        self.alloc_count = 0;
        // Aim to GC again after roughly doubling the live set.
        self.next_gc_threshold = (live * 2).max(4096);
    }

    pub fn live_count(&self) -> usize {
        self.slots.iter().filter(|s| matches!(s, HeapSlot::Live(_))).count()
    }

    pub fn slot_count(&self) -> usize {
        self.slots.len()
    }
}
