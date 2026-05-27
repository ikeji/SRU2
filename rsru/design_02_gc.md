# rsru — GC とヒープ

仕様: `memo/spec_03_semantics.md §1.3`、`csru/object_pool.cc`。

## 1. csru での実装

- `BasicObject` 内に `gc_counter` を持ち、`BasicObjectPtr` の生成/破棄で
  ±1 する（参照カウント）。
- 循環参照対策として、別途オブジェクトプール (`ObjectPool`) が全
  `BasicObject` を握り、定期的に Mark&Sweep する。Mark フラグも
  `BasicObject` の内側にある。

問題:
- どこからでもカウンタが書き換わる前提なので、Rust の借用規則と相性が
  悪い。
- Mark ビットも内部にあるため、複数 GC スレッドや再帰的 mark の状態
  管理が密結合になる。
- 「ルートか否か」をオブジェクト自体に持たせる必要があり、ルート集合の
  列挙が暗黙。

## 2. rsru での方針

すべて `Heap` に集約する。`BasicObject` 自身は GC メタデータを持たない。

```rust
pub struct Heap {
    slots:  Vec<HeapSlot>,        // index = ObjId
    free:   Vec<ObjId>,           // 再利用可能スロット
    roots:  HashMap<ObjId, u32>,  // 外部ルート集合 (id -> refcount)
    marks:  bitvec::BitVec,       // marks[id] = 到達済み
    sym:    SymbolTable,
    config: GcConfig,
}

enum HeapSlot {
    Live(BasicObject),
    Free,
}
```

### 2.1 ルート集合 (外部参照カウント)

- Rust から `BasicObject` を握る側は `RootHandle` を持つ。
  ```rust
  pub struct RootHandle<'h> {
      id: ObjId,
      heap: &'h Heap,    // 実際は &mut でなく &、内側で Cell/Atomic で操作
  }
  impl Drop for RootHandle<'_> { ... }    // refcount -= 1、0 で roots から除去
  ```
- `Heap::root(id)` で取得、Drop で解放。
- 「ルートでなくなった = 解放対象候補」だが、すぐには解放しない。次の Mark
  &Sweep サイクルで未参照なら回収。

### 2.2 Mark ビットの外出し

- `Heap::marks` は `slots` と同じ長さ。`marks[id]` が GC 1 回ごとに
  clear → 到達済みに set される。
- `BasicObject` には一切持たせない。これにより `&BasicObject` を取った
  まま `Heap` の他処理を呼べる場面が増え、Rust の借用とも噛み合う。

### 2.3 Mark&Sweep の流れ

```rust
fn gc(&mut self) {
    self.marks.fill(false);
    let roots: Vec<ObjId> = self.roots.keys().copied().collect();
    for id in roots { self.mark(id); }
    for id in 0..self.slots.len() {
        if let HeapSlot::Live(_) = &self.slots[id] {
            if !self.marks[id] {
                self.slots[id] = HeapSlot::Free;
                self.free.push(id as ObjId);
            }
        }
    }
}

fn mark(&mut self, id: ObjId) {
    if self.marks[id] { return; }
    self.marks.set(id, true);
    // fields の値、data の内側 ObjId を辿る
    let children = self.children_of(id);
    for c in children { self.mark(c); }
}
```

- `children_of` は `fields` の値全部 + `ObjData` バリアントごとの内側
  `ObjId`（`Array`、`Frame.binding/upper_frame/local_stack/expressions`、
  `Proc::Sru.definition_binding/body_ast` など）。
- 深い構造でも Rust のスタックがあふれないように、`mark` は明示スタックで
  反復にしておく。

### 2.4 GC 起動契機

- 「アロケーションが N 回起きたら次のループ境界でトリガ」程度の素朴な
  方針で十分（csru も類似）。`GcConfig::alloc_threshold` で持つ。
- 明示 GC API (`heap.collect()`) も提供。テストでルート漏れを検出しやすく
  する。

## 3. 借用の壁を越える

ハンドル方式の旨味は、評価器が `Heap` を `&mut` で握ったまま、複数の
`ObjId` を取り回せる点。例: `Heap::call_method(self, recv: ObjId, method:
SymbolId, args: &[ObjId])` のような API なら `&mut Heap` を渡すだけで済む。

注意: 評価中に新規オブジェクトを作るたびに `Heap` を `&mut` で借りるので、
評価器側で「現在の StackFrame をどこに置くか」が課題になる:

- **案**: `StackFrame` 自体を heap オブジェクトとして所有する。評価器は
  `current_frame: ObjId` のように ID を握り、毎回 `heap.frame_mut(id)` で
  取り出す。`Heap` の `&mut` 借用と局所処理の借用が衝突しないように、
  frame の修正は短い関数に閉じ込める。
- これは csru でも `StackFrame` を `BasicObject` として扱っているのと
  同じ路線。

## 4. ルート漏れに対する備え

- `RootHandle` の Drop でしか refcount を減らさない → ルート漏れは検出
  しやすい（プロセス終了時の roots 残量を assert）。
- `Heap::debug_root_dump()` を用意してテストで使う。

## 5. パフォーマンス想定

- 段階 A の時点では「GC は遅くてもよい」を許容する。`HashMap<ObjId, u32>`
  でも roots 数は数百〜数千程度のはずなので、十分動く。
- 後で `BitVec` を `Vec<u8>` に落とす、`free` を世代別管理にする、等の
  最適化はこの設計の上で差し替えで済む。
