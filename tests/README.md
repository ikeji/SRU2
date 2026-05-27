# SRU 結合テスト

`rsru` の実装の参照基準として、csru で動くサンプルを集めたもの。各 `*.sru`
について同名の `*.expected` を置き、両方を `run.sh` が走らせて比較する。

## 実行

```
$ tests/run.sh                          # csru で全テスト
$ tests/run.sh --include-extensions     # parser_extension も含めて実行
$ tests/run.sh --include-rsru-only      # rsru_only も含める (csru では失敗)
$ tests/run.sh path/to/rsru             # 別のインタプリタで実行
```

`csru` の `sru` バイナリは共有ライブラリ `libsru.so` に依存しているので、
`run.sh` は自動的に `LD_LIBRARY_PATH=csru/` を立てる。

## カバレッジ

| ファイル | 何を確かめるか |
| --- | --- |
| `fizzbuzz.sru` | while ループ、if/elsif/else、`%`、`toS`、`puts` |
| `arithmetic.sru` | 四則・剰余・優先順位、比較演算、整数/実数除算、文字列連結 |
| `array_basic.sru` | `Array.new()`, `push`, `at`, `size`, 配列リテラル |
| `classes_basic.sru` | `class C ... end`、`initialize`、`self.field`、メソッド呼び出し |
| `classes_inherit.sru` | `class Sub < Super` での上書き、継承されたメソッド |
| `closure_counter.sru` | 外側スコープ捕捉（`n = n + 1` が親 binding を上書きする） |
| `custom_truthy.sru` | 独自クラスに `ifTrue` / `ifTrueFalse` を定義して `if` の被演算子として使う |
| `recursion.sru` | `def` 内自己参照、相互再帰 |
| `continuation_return.sru` | `def` ボディの `return` を早期脱出として使う |
| `continuation_save.sru` | `return(return)` で継続を取り出して再侵入する |
| `bst.sru` | 二分探索木（自己参照ノード、再帰メソッド、メソッドチェイン） |
| `polymorphism.sru` | 親クラスのメソッドが self 経由で子のオーバーライドを呼ぶ |
| `church.sru` | Church 符号化したブール値で高階クロージャ |
| `parser_extension/hex_literal.sru` | `__parser.const_literal` を SRU 側で差し替えて 16 進リテラル `0x10` を後付け（csru でも動く） |
| `rsru_only/prelude_methods.sru` | rsru の SRU プレリュード由来のメソッド (Array#each/map/select/inject/join, Numeric#abs/negate/times) を使う |

## SRU の現実的な書き味メモ

テストを書く中で確認した csru の癖。仕様 (`memo/spec_*.md`) と差分が
ある箇所はおいおい仕様側を直す。

- `while (cond) ... end`: 条件は **必ず** カッコで囲む必要がある（仕様の
  記述より厳格）。
- `if cond then ... end` の `then` は省略可で、`\n` か `;` で代用できる。
- `puts (x).toS()` は `puts(x).toS()` と解釈され、`puts` の戻り値に `toS`
  を呼ぼうとする。`puts (x.toS())` か中間変数を使う。
- 引数なしのメソッド呼び出しでも `()` を省くと **呼び出されず proc が
  そのまま返る**。例えば `Array.new` は Proc を返し、`Array.new()` で
  インスタンスを得る。
- 識別子は `then` / `else` で **始めてはいけない**。レキサが予約語として
  greedy に食ってしまう（`thenP` → `then` + `P` で失敗）。
- 真偽値定数は `true` / `false`。組み込みコードでは `tlue` / `farse`
  という変数名が使われているが、これは C++ の予約語衝突回避のためで、
  SRU からは普通に `true` / `false` で参照する。
- `puts` は **String 専用**。Numeric を渡すと暗黙 `.toS()` はせず、
  空文字を出力する。`p` は何でも `Inspect` で表示する。
- 多くの `Array` メソッド（`pop`, `shift`, `each`, `join`, `flatten` 等）
  は csru では `CHECK(false)` の stub。動くのは `push`, `at`, `get`,
  `set`, `size`, `ltlt`（`<<`）, `slice`, `clear` ぐらい。
- `Numeric` リテラル `123` は AST 上では `Numeric.parse("123")` の
  CallExpression。`Numeric.parse` を再定義するとリテラルの意味が
  変わる（仕様 §spec_02_grammar §4.5）。
- 継続 (`return(return)` パターン) はトップレベルで使うと巻き戻し先が
  ずれて動かない。`{ ... }()` のクロージャ内で使うと期待通り動く。

## rsru への期待

- 上記の **全テストを通す** ことが段階 A（AST 直 walk）完了の合格条件。
- `parser_extension/` も `tests/run.sh --include-extensions` で通る
  ようにすれば、パーサーがリテラル追加に対して開かれている設計だと
  確認できる。
- `csru` と diff した出力差は仕様バグの疑い。先に `memo/spec_*.md` を
  実装に合わせて直す。
