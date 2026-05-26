# SRU 言語仕様 — 概要

このドキュメント群は、`csru/` にある C++ 実装の SRU インタプリタの
言語仕様と組み込み機能を整理したもの。事実は `csru/` のソースコードから直接抽出している。

## ドキュメント構成

| ファイル | 内容 |
| --- | --- |
| `spec_00_overview.md` | 全体像（このファイル） |
| `spec_01_lexical.md` | 字句構造（識別子・数値・文字列・コメント・空白） |
| `spec_02_grammar.md` | 構文（BNF 風）、演算子の優先順位、AST への糖衣展開 |
| `spec_03_semantics.md` | オブジェクトモデル、スコープ、評価モデル、継続 |
| `spec_04_builtins.md` | 組み込みクラス、ネイティブ関数 |
| `spec_05_runtime.md` | インタプリタの初期化と起動、REPL、`require` / `load` |

既存メモ (`syntax.txt`, `scheme-like.txt`, `stack_frame.txt`, `naming.txt`, `tutorial.txt`) も
背景知識として参照されたい。本仕様はそれらを補強・整合化した最新版である。

## 言語の特徴

- **すべてがオブジェクト**: 数値・文字列・配列・クラス・継続も含めてすべて `BasicObject`（スロット辞書 + 任意のネイティブ `Value`）として表現される。
- **プロトタイプ + クラスのハイブリッド**: 各オブジェクトは `klass` スロットで自身のクラスを指し、`klass` は `superclass` の連鎖で継承を表す。インスタンスメソッドはクラスの `instanceMethods` スロットに置く。
- **クロージャ第一級**: 関数定義はすべて無名クロージャの糖衣で、`def f(...)` は `f = {|...| ...}` に展開される。
- **継続をサポート**: クロージャの引数列に `:returnVar` を書くと、コンパイル時に「現在のスタックフレームを退避した継続」が `returnVar` に束縛される。これを呼ぶことでスタックを巻き戻せる。
- **コールスタックは連結リスト**: `StackFrame` オブジェクトが `upper_frame` を辿るリンクドリストを構成しており、継続の作成・呼び出しはフレームの「コピー」操作で実現される。
- **GC は参照カウント**: `BasicObjectPtr` がスマートポインタとして `IncrementGcCounter` / `DecrementGcCounter` を呼ぶ。循環参照は到達可能性 (`Mark`) と組み合わせて回収する設計。
- **パーサ自体が SRU オブジェクト**: 字句解析・構文解析の各ノードは Parser ジェネレータ (`csru/parser_generator/`) によって `__parser` オブジェクトのスロットに格納された手続きとして実装される。実行時には `__parser.parse(src)` を SRU 評価器自身が評価する。

## 実装上の構造メモ

- `Value` を抽象基底にして RTTI (`dynamic_cast`) でディスパッチする型システム。ネイティブデータ持ち (`SRUNumeric`, `SRUString`, `Array`, `Proc`, `StackFrame`, `Expression` 派生) は全部 `Value` のサブクラス。
- 評価モデルは「AST を `TraceVisitor` で平坦化 → `EvalVisitor` で一段ずつ評価」する 2 段方式。再帰呼び出しではなく値スタックを使うので、コール深さは C スタックではなく `StackFrame` の `upper_frame` リンクで決まる。
- ネイティブ手続きは `DEFINE_SRU_PROC` / `DEFINE_SRU_PROC_SMASH` の 2 種類。後者は呼び出し側の `StackFrame` を直接書き換えて尾呼出し的な制御移譲を行うためのもの。`Numeric.times`, `Array.each`, `Proc.whileTrue` 等の制御構造ヘルパに使われる。
