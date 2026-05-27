# パーサー拡張テスト

新しい字句種をパーサーにアドホックに足せるか、という設計レベルのテスト。
csru には `__parser` オブジェクトがランタイムから書き換え可能な形で
露出しているので、`hex_literal.sru` は **csru でも実際に動く**。

rsru でも同等の差し替え可能性を保ち、このスクリプトが同じ stdout を
出すこと。

## 1. 追加するリテラル: 16 進整数

`0x[0-9a-f]+` を整数リテラルとして受理する。脱糖先は通常の整数リテラルと
同じ `Numeric.parse("<decimal>")` の CallExpression なので、AST バリアント
は増えない。

## 2. csru での実装の勘どころ

- **`__parser.const_literal` を差し替える**。文法は
  `const_literal <= real | number | const_string` で、ここが「リテラル」
  の選択肢を集めるレイヤ。新しい選択肢を足したい場合は、この規則を
  先頭に差し込むのが自然。
  - 試行錯誤メモ: `__parser.number` を差し替えても効かない。元の `real`
    が `[0-9.]+` を greedy にマッチするので、整数リテラルでも `real` で
    吸収されてしまうため。`real` を hook する手もあるが、内部順序に依存
    していて気持ちよくない。`const_literal` レイヤで足すのが本来の筋。
- **戻り値は素の Object**。`Object.new()` で `status` / `pos` / `ast`
  を持つオブジェクトを作って返せば、parser_util.cc の `CreateTrue` と
  同じ shape になる。
- **AST は借りる**。csru の AST クラスは `AST.LetExpression` 等として
  ユーザ空間に名前は見えているが、`.new()` を実装しているのは
  `StringExpression` だけ（`ast_class.cc:48`）。残りはクラスオブジェクト
  だけが置いてあって、`new()` を呼ぶと内部の `Value` サブクラスインスタンス
  を持たない空の BasicObject が返り、評価器で落ちる。
  なので CallExpression を SRU から組み立てる手段は実質ない。代わりに
  `__parser.parse("<decimal>").ast` を流用して、`123` リテラルが普段
  作っているのと同じ AST を借りる。
- **再帰の心配なし**。差し替え後の `const_literal` の中で `__parser.parse`
  を呼ぶと内側でもこの新 const_literal が走るが、内側のソースは
  `<decimal>` で `0x` プレフィクスを含まないので無限再帰にはならない。

## 3. rsru への要請

このテストを通すには、rsru も以下を満たす必要がある:

1. パーサーの各規則が **オブジェクトのスロット** として外から差し替え
   可能であること（csru の `__parser.<rule>` モデルを踏襲）。あるいは
   同等の登録 API を出すこと。
2. パース結果の形 (`{ status, pos, ast }` を持つ素のオブジェクト) を
   外部規則が返した場合に内部規則と同じ流れに乗せられること。
3. `__parser.parse(src)` が AST を返し、それを別の規則の戻り値として
   利用できること。

これらは「リテラルを 1 つ足す」という最小単位の拡張で必要十分。
**ついでに改善する余地**: rsru では `AST.CallExpression.new(recv,
method, args)` のような完全な AST コンストラクタを公開しておくと、
パーサー拡張側で `__parser.parse` の借用が不要になり、もう少しまとも
な書き味になる（csru の `ast_class.cc` の制約は、移植のついでに直して
よい点）。

## 4. ファイル

| ファイル | 内容 |
| --- | --- |
| `hex_literal.sru` | パッチ + 使用例。csru でそのまま動く |
| `hex_literal.expected` | stdout 期待値 |

## 5. テスト合格条件

```
$ tests/run.sh --include-extensions
# parser_extension/hex_literal --> PASS （csru でも rsru でも）
```
