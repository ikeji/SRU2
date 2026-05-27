# rsru — バイトコード

## 1. 目的と非目的

**目的**: `sru_lib/*.sru` を **ビルド時にコンパイル**して `include_bytes!`
で同梱できるようにする。実行時パースを省くことで起動を速め、組み込み
ライブラリのソースを最終バイナリに同梱したくないケースに備える。

**非目的**: 実行高速化（JIT、レジスタ機の精緻化、インライン展開など）。
速度のためではなく、組み込みイメージのために導入する。

## 2. AST との関係

仕様 (`memo/spec_02_grammar.md`) の AST は次の 5 種類:

```
LetExpression, RefExpression, CallExpression, ProcExpression, StringExpression
```

**唯一のリテラル種は `StringExpression` だけ**。数値リテラル `123` は
パース時に `Numeric.parse("123")` の CallExpression に変換される（仕様
§spec_02_grammar §4.5）。これがバイトコード設計上の重要な制約。

## 3. バイトコード形式

仕様の AST に 1 対 1 で対応する命令 5 種 + 補助:

| OP | オペランド | スタック効果 | 対応 AST / 用途 |
| --- | --- | --- | --- |
| `LDS  k` | const_pool[k]: 文字列 | push StrObj | StringExpression |
| `REF  s, has_env` | symbol id, env 有無 | (`env? pop` →) push value | RefExpression |
| `LET  s, has_env` | symbol id, env 有無 | (`env? pop` →) pop value → push value | LetExpression |
| `CALL m, argc, has_recv` | method sym, 引数数, recv 有無 | pop args[argc] (+recv) → push result | CallExpression |
| `PROC k` | proc_table[k]: ProcDef | push Proc | ProcExpression |
| `POP` | — | pop discard | 文の値捨て |

- `LET` のスタック動作は「最後に **値を残す** （代入式は値を持つ）」とする。
  csru でも代入は値を返す（`a = b = 1` のように）。
- `CALL` は受け手有り無しを区別。`has_recv=true` のときは「recv args...」の
  順でスタックに積んであり、`CALL` が pop して呼び出す。

これは csru の AST 5 種を素直に並べたものなので、**AST と意味論が完全に
一致** する。

## 4. 定数プールとプロックテーブル

```rust
pub struct Module {
    pub strings:  Vec<String>,        // LDS のターゲット
    pub symbols:  Vec<String>,        // REF/LET/CALL のシンボル
    pub procs:    Vec<ProcDef>,       // PROC のターゲット
    pub top:      Vec<Insn>,          // トップレベルの命令列
}

pub struct ProcDef {
    pub vargs:    Vec<u32>,           // symbols インデックス
    pub retval:   Option<u32>,
    pub body:     Vec<Insn>,
}
```

- 命令ストリーム内のシンボル参照は `symbols` のインデックス。実行時に
  `SymbolTable::intern(symbols[i])` で内部 ID に解決する（モジュール読込
  時にまとめて変換しておく方が速い）。
- 数値リテラル専用の定数プールは **無い**。`123` は次のコード列に展開
  される:
  ```
  REF  Numeric, has_env=false
  LDS  "123"
  CALL parse, argc=1, has_recv=true
  ```
  これにより `Numeric.parse` をユーザがオーバーライドした場合も AST 直接
  評価と同じ結果になる。

## 5. オプション最適化: `--const-fold`

**デフォルトでは数値リテラルの意味が変わらない**ことが最優先。が、`Numeric
.parse` を上書きしないことが分かっているコードでは `123` を毎回呼び出すの
は無駄。そこでオプトインの定数畳み込みを設ける。

`rsruc --const-fold input.sru -o out.bc`:

- パース後、`Numeric.parse("123")` 形の Call を **コンパイル時に評価**
  して結果オブジェクトを「定数オブジェクトプール」に格納する。
- 新しい命令を 1 つ追加: `LDC k`（const_pool[k]: 任意オブジェクト）。
  Numeric リテラルの位置はこれに置き換える。
- これは **副作用** として、その後 SRU 側で `Numeric.parse = { ... }` と
  しても、定数畳み込みされたリテラルには反映されなくなる。だから
  オプトイン。

ユースケース:
- `sru_lib/*.sru` のコンパイル時は `--const-fold` を有効にしてよい。
  組み込みライブラリ内では `Numeric.parse` を再定義しないと約束できる
  範囲なので、安全。
- ユーザのスクリプトは原則オフ。

## 6. コンパイラの構成

```
src/bytecode/
  format.rs     — Module, ProcDef, Insn 定義 + シリアライズ
  compiler.rs   — &[Expression] → Module
  vm.rs         — Module を読んで評価器に流し込む
  emit.rs       — file format への encode/decode
```

`compiler::compile(program: &[Expression]) -> Module` の流れ:

1. 文字列/シンボル/proc のテーブルを作る `Builder`。
2. 各 `Expression` について `emit(expr, &mut insns)` を呼ぶ。
3. ProcExpression に当たったら、その body を再帰的にコンパイルして
   `ProcDef` を `procs` に push、`PROC k` を出す。
4. トップレベルの文末には `POP` を入れる。最後の文だけは `POP` を省く
   ことで「式の値が残る」ようにする。

`Insn` のエンコードは可変長:

```
opcode: u8
operands: 命令ごとに固定長 (LEB128 か u32 ベタ)
```

シンプルさ重視で `u32` ベタにしておく。Module 全体を bincode で書く案も
可能（速いし楽）。

## 7. VM

`design_04_evaluator.md` の Trace+Eval モデルと同形にする。

- `Module` を読み込んで `Insn` 列を `TraceOp` 列に **1 対 1 変換**する。
- そこから先は AST 直 walk と同じ評価ループに流す。

つまり VM 専用の評価ループは作らない。バイトコードは **AST のシリアラ
イズ形式** として扱う。これで:

- 段階 A の AST 評価器がそのまま動く。
- AST 評価 / バイトコード読み込みの **挙動の食い違い** を最小化できる
  （変換層が薄い）。

## 8. モジュールのリンクと配置

- `Module` ごとに独立にエンコード可能。
- `prelude` を構成する複数の `*.sru` モジュールは、build.rs が個々に
  コンパイルし、`Vec<Module>` として埋め込む（または事前に全部を 1
  モジュールに結合する）。
- 結合バイナリ起動時の流れ:
  1. `BUILTIN_IMAGE: &[u8] = include_bytes!("builtin.bc");`
  2. `let modules = format::decode(BUILTIN_IMAGE)?;`
  3. ブートストラップ後、`for m in modules { vm::run(heap, m); }`。
  4. その後にユーザスクリプトを評価。

## 9. テスト

- AST 直 walk と バイトコード経由 で `samples/*.sru` を回して結果一致を
  確認するスナップショットテスト。
- `--const-fold` あり/なしで結果が一致するか（`Numeric.parse` を再定義
  していない範囲で）。
- Round-trip テスト: `compile -> encode -> decode -> run` の各段で結果が
  同じ。

## 10. 仕様外への拡張余地

将来必要になったら、以下は **後から追加できる** ように設計しておく:

- 数値リテラル専用 op（`LDI i64`, `LDF f64`）と `--const-fold` の融合。
- ジャンプ命令 (`JMP`, `JZ`, `JNZ`) を入れて `if`/`while` の Native
  Smash 経由を省く（仕様 §脱糖を変えないと整合が崩れるので、慎重に）。
- 行番号テーブル（デバッガ向け）。

これらは現状不要なので入れない。最初の版は **AST と 1:1** を守る。
