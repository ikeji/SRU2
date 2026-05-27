# rsru — オブジェクトモデル

仕様: `memo/spec_03_semantics.md §1`。

## 1. `BasicObject` の Rust 表現

```rust
pub struct BasicObject {
    pub fields: HashMap<SymbolId, ObjRef>,
    pub data:   Option<ObjData>,
}

pub type SymbolId = u32;       // symbol テーブル経由で割り当て
pub type ObjRef   = ObjId;     // = u32。ヒープ内のインデックス
```

- スロット辞書は `HashMap<SymbolId, ObjRef>`。csru の `unordered_map<int,
  BasicObject*>` と同じ意味。
- ネイティブデータは `Option<ObjData>`（後述 §3）。
- 参照カウンタとマークビットは **持たない**。これらは `Heap` 側に外出し
  する（`design_02_gc.md`）。

## 2. `ObjRef` をハンドル方式にする理由

csru は生ポインタ `BasicObject*` を `BasicObjectPtr`（RAII で内側のカウン
タをいじる）で包んでいる。Rust で同じことを `Rc<RefCell<BasicObject>>` で
やると:

- スロット辞書から取り出した値が `RefCell` 借用を持ち続け、`obj.fields[k]
  .borrow().fields[k2]` のような連鎖で `BorrowError` を踏みやすい。
- マーク&スイープ用に「全オブジェクトの集合」を持つには弱参照の登録が要る。

ハンドル方式 (`ObjId`) を採ると:

- スロットから取り出すのはただの `u32`。借用の連鎖が起きない。
- `Heap` がオブジェクト全列を所有するので、マーク用に全列挙が自明。
- 継続のために `StackFrame` をコピーするのも、フレームが `ObjId` を抱える
  だけなので安価。

## 3. ネイティブデータ `ObjData`

csru の `Value` サブクラスに対応。Rust では `enum` で表現する:

```rust
pub enum ObjData {
    Num(NumVal),                 // 整数 or 実数 (SRUNumeric)
    Str(String),                 // SRUString
    Array(Vec<ObjRef>),          // Array
    Proc(ProcKind),              // Proc / NativeProc / NativeProcWithSmash
    Frame(StackFrame),           // 継続が握るフレーム
    Expr(Expression),            // AST ノード（eval(ast) 用）
}

pub enum NumVal {
    Int(i64),
    Real(f64),
}

pub enum ProcKind {
    Sru(SruProc),                // SRU で書かれた手続き
    Native(NativeFn),            // 即時実行型
    NativeSmash(NativeSmashFn),  // フレームを差し替える型
}
```

- `Inspect` は `ObjData` の variant ごとに `match` で実装。
- マーク時は variant ごとに内側の `ObjRef` を辿る（`Array`, `Frame`,
  `Proc::Sru` の binding 等）。csru の `Value::Mark()` 相当。
- 整数/実数の使い分けは csru の `SRUNumeric` に揃える: 整数同士の演算で
  剰余 0 なら整数、それ以外は実数（`memo/spec_04_builtins.md §Numeric`）。

## 4. Value トレイトは要らない

csru の `Value` 抽象クラスは継承で multimethod を実現していたが、Rust では
`enum` + `match` で十分。トレイトオブジェクトにする旨味は

- 拡張性（他クレートから ObjData バリアントを増やしたい）

くらいで、本実装では不要。native プラグインが扱う「自前データ型」は
`ObjData::Native(Box<dyn Any + Send>)` を必要になったら追加することで対応
する（段階 D 以降）。

## 5. クラス階層と組み込みオブジェクト

仕様 `memo/spec_03_semantics.md §2`、`memo/spec_04_builtins.md §3` のクラ
ス階層を Rust 側で組み立てる:

1. `Heap::new()` で空のヒープを作る。
2. `bootstrap::install_classes(heap, root_binding)` が `Class`, `Object`,
   `Nil`, `Boolean`, `Numeric`, `String`, `Array`, `Hash`, `Proc`,
   `Binding`, `Math`, `Sys` を順に生成し、`klass`/`superclass`/
   `instanceMethods` スロットを埋めて root binding に束縛する。
3. 各クラスの `instanceMethods` に `ObjData::Proc(Native(...))` を直接挿入
   する。組み込みメソッドのうち SRU で書けるものは段階 B で
   `sru_lib/*.sru` に移す。

## 6. シンボルテーブル

```rust
pub struct SymbolTable {
    forward: HashMap<String, SymbolId>,
    reverse: Vec<String>,
}
```

- グローバル singleton（`OnceLock<RwLock<SymbolTable>>`）。csru と同じく
  プロセス内で共有。
- ビルド時に既知シンボル（`klass`, `superclass`, `instanceMethods`, ...）
  をプリインターンして、`sym::klass()` のような関数を `build.rs` で生成
  すれば csru の `constants.cc` 相当になる（必須ではない、段階 A では
  動的インターンで足りる）。

## 7. nil の扱い

- `Nil` は唯一のインスタンスを持つシングルトン（仕様 §2.2）。
- 「未初期化スロット参照」「引数が足りない」「`return` 値なし」等は全部
  この `nil` を返す。
- `ObjId` ではなく `Option<ObjRef>` で nil を表現する誘惑があるが、SRU 上
  からは nil も普通のオブジェクトなので **必ず実体を持つ** ことにする。
  Rust 側で `None` と nil を混同しない。
