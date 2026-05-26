# SRU 言語仕様 — 意味論

ソース: `csru/basic_object.{h,cc}`, `csru/binding.cc`, `csru/class.cc`,
`csru/object.cc`, `csru/stack_frame.cc`, `csru/interpreter.cc`, `csru/proc.cc`,
`csru/library.cc`。

## 1. オブジェクトモデル

### 1.1 BasicObject
- すべての SRU 値は `BasicObject` のインスタンス（`csru/basic_object.h`）。
- フィールド:
  - `fields` : `symbol_id (int) -> BasicObject*` のハッシュ（`unordered_map`）。これが「スロット」。
  - `data`   : ネイティブ拡張データ（`Value*`）。`SRUNumeric` / `SRUString` / `Array` / `StackFrame` / `Proc` / `Expression` などの実体はここに保持される。
  - `gc_counter` : 参照カウント。`BasicObjectPtr` の生成・破棄に連動。
- 主な操作:
  - `Set(name, ref)` / `Get(name)` / `HasSlot(name)` … スロット辞書のアクセス。
  - `Mark()` … `fields` の値と `data->Mark()` を再帰呼び出し（到達可能性ベース GC 用）。
  - `Inspect()` … デバッグ表示。`Value` が定義していれば優先される（`Numeric(3)`, `String("...")` 等）。

### 1.2 値（ネイティブデータ）
ネイティブデータを持つ型は `Value` を継承し、以下のメソッドを実装する:

- `Dispose()` … 破棄時の自己解放フック。
- `Mark()`    … 自身が指している `BasicObject*` をマーク。
- `Inspect()` … 文字列化。

主なサブクラス:
`SRUNumeric` `SRUString` `Array` `Proc` (`SRUProc` / `NativeProc` / `NativeProcWithSmash`)
`StackFrame` `Expression` の派生 (`LetExpression` 等)。

### 1.3 GC モデル
- 各 `BasicObjectPtr` がカウンタを上下させる（`basic_object.h:184-235`）。
- 循環参照は到達可能性 GC（`Mark` を辿る）と組み合わせて対処する設計（実装はオブジェクトプール側 `object_pool.cc`）。

## 2. クラスシステム

### 2.1 スロットの取り決め
- `klass`            : インスタンスから自身のクラスを参照。Class 自身も `klass = Class`。
- `superclass`       : クラスから親クラスを参照。`Object.superclass = nil`。
- `instanceMethods`  : 各クラスが持つメソッド辞書（`BasicObject`）。
- `__name`           : クラス／インスタンスの表示名（`String`）。
- `__findSlot`       : スロット未発見時に呼ばれるメソッド（既定はクラス階層の `findInstanceMethod` を呼ぶ）。
- `findInstanceMethod`: メソッド検索手続き（クラスチェーンを辿る）。
- `findSlotMethod`   : Class オブジェクトに置く `__findSlot` のひな型。

これらは `class.cc::InitializeInstance` / `SetAsSubclass` / `SetAsInstanceMethod` で初期化される。

### 2.2 クラス階層
`library.cc:108-174` で次のように構築される。

```
Class
  └── Object
        ├── Proc        ← Class, Binding 等の親はここに繋がる
        ├── Array
        ├── Hash
        ├── String
        ├── Numeric
        ├── Boolean
        │     ├── True (single instance, klass=Boolean)
        │     └── False(single instance, klass=Boolean)
        ├── Math
        └── Sys
```

- `Class.klass = Class`（自己参照）。
- `Class.superclass = Object`、`Object.superclass = Nil`。
- `Nil` は `Object` をクラスとして共有するシングルトン値（`csru/nil.cc:15`）。

### 2.3 インスタンス化
- `someClass.new(args...)` が標準。`class.cc::object_new` がインスタンスを `BasicObject` として生成し、`klass` を設定し、`self.initialize(args...)` が定義されていれば呼ぶ。
- Class が持つ組み込みメソッド: `subclass`（クラスを派生）, `mew` (= `new`)。

### 2.4 メソッド検索

`RefExpression` の評価時 (`stack_frame.cc:153-179`) :
1. 環境オブジェクト (`env`、省略時は現在 Binding) に `name` スロットがあればそれを返す。
2. 無ければ `env.__findSlot(env, name)` を呼ぶ。
3. 既定実装 `Class::findSlot` は `env.klass.findInstanceMethod(klass, name)` を呼ぶ。
4. `findInstanceMethod` は `klass.instanceMethods[name]` を返し、無ければ `klass.superclass` で再帰探索。
5. すべて失敗すれば nil を返す。

これにより `obj.foo` という形であっても、`obj` 自身のスロット → クラスのインスタンスメソッド → 親クラス、と Smalltalk 風の検索が行われる。

## 3. バインディング（変数スコープ）

### 3.1 Binding オブジェクト
`csru/binding.cc`。`Binding::New([parent])` で作成され、以下のスロットを持つ:
- `local`            : 自分自身を指す
- `_parent`          : 親 Binding（無ければ未設定）
- `__findSlot`       : `FindSlot` ネイティブ手続き（親チェーンを辿る）
- `get`              : `BindingGet`
- `set`              : `BindingSet`

通常のローカル変数は `Binding` オブジェクトのスロットに `Set` される。

### 3.2 スコープ規則（代入）
`stack_frame.cc:126-152` の `Accept(LetExpression*)` より:

1. 左辺の `env` が nil なら現在のバインディングを起点とする（普通の代入）。
2. 起点 `env` から `_parent` を辿って **既存スロット** を探す。
3. 見つかれば既存スロットに代入（上書き）。
4. 見つからなければ **起点の env** に新規スロットを作って代入（ローカル変数の宣言）。

つまり、外側のスコープに同名変数があれば外側を書き換え、無ければ局所変数を作る。Ruby と同様の動的スコープではなくレキシカルスコープ。

### 3.3 スコープ規則（参照）
`stack_frame.cc:153-179`:

1. 起点 env のスロットを直接見る。
2. 無ければ `env.__findSlot(env, name)` を呼び、Binding 用の `FindSlot` は `_parent` を辿りつつ各段で「直接スロット」「`__findSlot` のさらなる呼び出し」を試みる。
3. すべて失敗したら nil を返す（未定義変数の参照はエラーではなく nil）。

### 3.4 関数呼び出しと新規 Binding
`proc.cc::SRUProc::Call`:
- 新しい `Binding::New(parent=クロージャの定義時 binding)` を作る（**レキシカル**）。
- 仮引数 `varg[i]` を順に呼び出し引数で束縛。引数が足りなければ nil。
- `retval` が空でなければ、現在の **呼び出し元** スタックを保存した継続を `retval` 名で束縛する。
- 新しいスタックフレームを積み（`DigIntoNewFrame`）、本体の AST 配列を `Setup`。

## 4. 評価モデル

### 4.1 スタックフレーム
`csru/stack_frame.cc`。各フレームは以下を持つ:
- `expressions` : これから評価する AST のリスト
- `tree_it`     : 次に評価する AST のインデックス
- `operations`  : `TraceVisitor` で平坦化した実行ステップ列
- `it`          : 次に実行するステップ
- `local_stack` : 値スタック（評価結果が積まれる）
- `binding`     : 現在のバインディング
- `upper_frame` : 呼び出し元フレーム（連結リスト）

### 4.2 評価の単位
- 1 ステップ = `EvalNode()` の 1 回。
- AST ツリーを `TraceVisitor` がポストオーダー風に平坦化する: 子→親 の順で `operations` に積む。
- `EvalVisitor` は `operations` を 1 つずつ取り出し、対応する処理（`LetExpression`/`RefExpression`/`CallExpression`/`ProcExpression`/`StringExpression`）を `local_stack` に対して実行する。
- 関数呼び出し (`CallExpression`) では `Proc::Invoke` が呼ばれ、`DigIntoNewFrame` 経由で新しいフレームに切り替わる。値は新フレームの `local_stack` から、末尾に達した時点で **元フレームの local_stack に積まれる**（`EndOfTrees` 後の `Step Out`、`stack_frame.cc:270-290`）。

### 4.3 関数呼び出しのライフサイクル
1. `CallExpression` 評価で `Proc::Invoke(context, proc, args)`。
2. ネイティブ手続きなら即時実行（`DEFINE_SRU_PROC`）か、`StackFrame` 操作型なら `Setup` し直し（`DEFINE_SRU_PROC_SMASH`）。
3. SRU 関数 (`SRUProc`) なら、`Binding::New(definition_binding)` → `DigIntoNewFrame` → 引数を束縛、で新フレームを準備し、本体を順次評価。
4. 本体最後の値が `local_stack` 末尾。フレーム終端 (`expressions` も `operations` も使い切ったとき) で `upper_frame` に戻り、結果を呼び出し元の `local_stack` に push。

### 4.4 評価ループの不変条件
- 各文の評価結果は `local_stack` の末尾になる。
- 文末（フレーム末）に達したら結果を上位フレームに渡してフレームスイッチ。
- これにより **末尾呼び出しがスタック深さを増やさない** 構造になっている（C スタックではなく `upper_frame` リンクを操作するだけ）。

## 5. 継続 (continuation)

`csru/interpreter.cc:74-92`、`memo/scheme-like.txt` 参照。

- クロージャに `:retVar` がつくと、関数呼び出し時に「現在のスタックフレームを退避した継続オブジェクト」が `retVar` に束縛される。
- 継続オブジェクトは `ContinationInvoke` というネイティブ手続きで、内部に `currentStackFrame` スロット（呼び出し元フレームのコピー）を持つ。
- 継続を呼ぶと:
  1. 現在の `StackFrame` に保存していたフレームをコピー（`*Interpreter::Instance()->CurrentStackFrame() = *cur_frame`）。
  2. 引数があればそれを戻り値として返す（無ければ `nil`）。
- スタックは連結リスト構造なので、フレームを 1 つコピーするだけで巻き戻し可能。Scheme の `call/cc` 相当だが、コピー対象は 1 フレームのみ（フレーム自身が `upper_frame` リンクで親を保持しているため）。
- `def f(...) ... end` は `f = {|...:return| ...}` に展開され、関数本体は `return(value)` で抜け出すことができる。`return` を引数なしで呼び出した場合は `nil` を返す。

## 6. インタプリタの初期化

`csru/interpreter.cc:47-58` `InitializeInterpreter()`:

1. ルートとなる `StackFrame` を作り、新規 `Binding` を割り当てる。
2. `Library::BindPrimitiveObjects(binding)` でクラス・定数を束縛（`spec_04_builtins.md`）。
3. `SetupRequireFunction(binding)` / `SetupLoadFunction(binding)` / `SetupPrintFunction(binding)` で `requireNative`, `load`, `eval`, `p`, `print`, `puts`, `exit` を束縛。

## 7. エラー処理

- 専用の例外機構は無い。実装は `CHECK` / `DCHECK` で abort することが多い。
- パースエラーは `Eval(string)` の戻り値が NULL になり、`PrintErrorPosition` でソース上の位置を表示する。
- 実行時の不正値（ネイティブ手続きの型不一致など）は `CHECK` でプロセス停止。致命的なら abort し回復しない方針。

## 8. 同時実行・スレッド

- スレッドサポートは無い。`Library` も `Interpreter` も Singleton で、グローバル可変状態を保つ。

## 9. 既存コードに見える注意点

- `static BasicObjectPtr times_internal;` のように関数内 static で再利用される手続きが多い。スレッド非対応はこの辺にも現れている。
- `load(filename)` は内部で再帰的に `Eval` を呼ぶが、`Eval` は元来 re-entrant ではない（`load.cc` のコメントにも「TODO: This code is invalid」とある）。
