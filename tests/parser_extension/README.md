# パーサー拡張テスト

このディレクトリのテストは、**csru では実行できない**。`rsru` のパーサー
を「アドホックに新しいリテラルを足せる」構造で書けているか、を確かめる
ためのコントラクトテスト。

## 1. 追加するリテラル: 16 進整数

ソース上の `0x[0-9a-fA-F]+` を整数リテラルとして受理する。`123` という
整数リテラルが AST 上で

```
CallExpression(Numeric, parse, [StringExpression("123")])
```

に脱糖されるのと同じ手筋で、`0x10` は

```
CallExpression(Numeric, parse, [StringExpression("16")])
```

に脱糖する。

つまり:

1. 字句層に「`0x` + 1 個以上の 16 進数字」を 1 トークンとして認識する規則を
   足す。
2. パーサーは整数リテラル位置でその規則を試す。
3. 構文木への変換は **コンパイル時に 10 進文字列へ変換**して、いつもの
   `Numeric.parse(...)` の CallExpression に変える。

メリット:
- AST スキーマを増やさない（仕様 `memo/spec_02_grammar.md` の AST 5 種を
  保てる）。
- ランタイムの動作は既存の `Numeric.parse` 経由なので、副作用なく
  実行できる。

## 2. 期待挙動

`hex_literal.sru` を実行すると `hex_literal.expected` と同じ stdout を
出すこと。

## 3. csru での確認

csru では `0x10` は識別子先頭の `0` でパース失敗する想定。期待出力は
あくまで rsru が「拡張済み」の状態で出すべきもの。CI では `csru` 用には
このディレクトリを除外する。

## 4. なぜこの形にしたか

- 「リテラル種類を 1 つ足すだけ」という最小単位の拡張で、パーサー設計が
  モジュラーかどうかを評価できる。
- 既存の評価器・組み込みライブラリを **一切いじらず**に達成できることが
  ポイント。もし AST にバリアントを増やさないと足せない設計だったら、
  この程度の追加でも全体を触ることになる。
- 拡張に応じて `memo/spec_02_grammar.md` を更新する必要はない（オプト
  イン拡張という位置付け）。

## 5. テスト合格条件

```
$ rsru tests/parser_extension/hex_literal.sru > /tmp/out
$ diff /tmp/out tests/parser_extension/hex_literal.expected
# (no diff)
```
