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

- **差し替えるのは `__parser.real`**。const_literal は `real | number |
  const_string` の順に試し、元の `real` の `[0-9.]+` が整数も貪欲に
  マッチするので、`number` を hook しても `real` が先に取ってしまう。
- **戻り値は素の Object**。`Object.new()` で `status` / `pos` / `ast`
  を持つオブジェクトを作って返せば、parser_util.cc の `CreateTrue` と
  同じ shape になる。
- **AST は借りる**。SRU からは `StringExpression.new` 以外の AST 構築
  API が無いので、`__parser.parse("<decimal>").ast` を流用する。
- **再帰の心配なし**。差し替え後の `real` の中で `__parser.parse` を呼ぶ
  と内側でもこの新 real が走るが、内側のソースは `<decimal>` で `0x`
  プレフィクスを含まないので無限再帰にはならない。

## 3. rsru への要請

このテストを通すには、rsru も以下を満たす必要がある:

1. パーサーの各規則が **オブジェクトのスロット** として外から差し替え
   可能であること（csru の `__parser.<rule>` モデルを踏襲）。あるいは
   同等の登録 API を出すこと。
2. パース結果の形 (`{ status, pos, ast }` を持つ素のオブジェクト) を
   外部規則が返した場合に内部規則と同じ流れに乗せられること。
3. `__parser.parse(src)` が AST を返し、それを別の規則の戻り値として
   利用できること。

これらは「リテラルを 1 つ足す」という最小単位の拡張で必要十分。新規
AST バリアントもランタイム API も増やさずに済む。

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
