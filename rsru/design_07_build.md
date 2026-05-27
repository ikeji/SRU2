# rsru — ビルドと組み込みイメージ

## 1. クレート構成

```
rsru/
  Cargo.toml          — workspace ではなく単一クレート
  build.rs            — sru_lib/*.sru → bytecode 生成
  src/
    main.rs           — CLI (REPL / ファイル実行 / stdin 実行)
    lib.rs            — pub use ...
    heap/
      mod.rs          — Heap, ObjId, RootHandle
      gc.rs           — Mark&Sweep
      symbol.rs       — SymbolTable
    object.rs         — BasicObject, ObjData
    ast.rs            — Expression と pretty-printer
    parser/
      mod.rs
      lexer.rs
      expr.rs
      stmt.rs
      desugar.rs
      error.rs
    eval/
      mod.rs          — run(heap, frame)
      frame.rs        — StackFrame
      trace.rs        — TraceOp とトレース
      step.rs         — 1 ステップ実行
      cont.rs         — 継続 (ContinationInvoke)
    builtin/
      mod.rs          — bootstrap()
      class.rs        — Class
      object.rs       — Object
      nil.rs
      boolean.rs
      numeric.rs
      string.rs
      array.rs
      proc.rs
      binding.rs
      math.rs
      sys.rs
      io.rs           — p / puts / print / exit / load / eval / require
    bytecode/
      mod.rs
      format.rs       — Module / Insn
      compiler.rs
      vm.rs           — Insn → TraceOp 変換
      emit.rs         — シリアライズ
    bin/
      rsruc.rs        — オフラインコンパイラ (任意)
  sru_lib/
    prelude.sru
    bool.sru
    numeric.sru
    array.sru
    ...
  tests/
    parser/
    eval/
    samples/          — csru/samples をシンボリックリンク or copy
```

`bin/rsruc.rs` は必須ではない。`build.rs` 内でも同じコンパイラを呼べる。

## 2. Cargo.toml

```toml
[package]
name = "rsru"
version = "0.1.0"
edition = "2021"

[dependencies]
bitvec = "1"

[build-dependencies]
# build.rs から自クレートのコンパイラを呼ぶための再エクスポート用
# 実体は src/parser, src/bytecode を [build-dependencies] 用に
# サブクレート化することで解決する（下記 §3）。
rsru-compiler = { path = "compiler" }

[features]
default = []
editline = []      # REPL の editline 対応
const-fold = []    # rsruc が --const-fold をデフォルト ON にする
```

メイン依存は最小（`bitvec` のみ）。`Box<dyn Any>` や手書きのもので大体
こと足りる。

## 3. ブートストラップ問題: build.rs が SRU コンパイラを使う

`build.rs` から `sru_lib/*.sru` をバイトコードに変換するために、SRU
コンパイラ（パーサ + 落とし）が必要。これを **どこに置くか** が論点:

### 案 A — 同クレートに置く（NG）

`rsru` 本体クレートのコードを `build.rs` から呼ぼうとすると、build.rs は
`rsru` 自身がコンパイルされる前に走るので使えない。

### 案 B — サブクレート化（推奨）

```
rsru/
  Cargo.toml          (rsru)
  compiler/
    Cargo.toml        (rsru-compiler: parser + bytecode のみ)
    src/
      lib.rs
      parser/         (内容は src/parser/* を共有)
      bytecode/
  src/
    lib.rs            (rsru: rsru-compiler を依存に含む)
```

- パーサとバイトコードコンパイラは `rsru-compiler` クレートにし、Heap や
  Evaluator は持たない。
- `rsru` クレートは `rsru-compiler` を **dependencies** にも **build-
  dependencies** にも入れる。前者は ランタイムでユーザコードを compile
  するため、後者は build.rs から組み込みイメージを作るため。
- 評価器・組み込みライブラリは `rsru` 側にのみ存在。コンパイラがそれら
  を必要としないので循環依存にならない。

これにより build.rs から `rsru_compiler::compile_file(path)` を呼べる。

## 4. build.rs の仕事

```rust
fn main() {
    println!("cargo:rerun-if-changed=sru_lib");
    let out_dir = std::env::var("OUT_DIR").unwrap();
    let order = std::fs::read_to_string("sru_lib/prelude.order").unwrap();
    let mut modules = vec![];
    for file in order.lines().filter(|l| !l.is_empty() && !l.starts_with('#')) {
        let path = format!("sru_lib/{file}");
        let m = rsru_compiler::compile_file(&path)
            .expect(&format!("failed to compile {path}"));
        modules.push(m);
    }
    let bytes = rsru_compiler::encode_modules(&modules);
    std::fs::write(format!("{out_dir}/builtin.bc"), bytes).unwrap();
}
```

- ファイル順は `sru_lib/prelude.order` というテキストファイルで管理。
  これによりロード順を明示できる。
- 出力は `$OUT_DIR/builtin.bc`。ランタイムでは `include_bytes!(concat!
  (env!("OUT_DIR"), "/builtin.bc"))` で参照。

## 5. ランタイムでの組み込みイメージのロード

```rust
const BUILTIN_BC: &[u8] = include_bytes!(concat!(env!("OUT_DIR"), "/builtin.bc"));

pub fn boot(heap: &mut Heap, root_binding: ObjId) {
    builtin::install_primitives(heap, root_binding);   // §design_05 §3
    let modules = bytecode::decode(BUILTIN_BC).expect("corrupt builtin image");
    for m in modules {
        let frame = bytecode::vm::frame_for_module(heap, root_binding, &m);
        eval::run(heap, frame);                        // §design_04 §7
    }
}
```

## 6. ランタイム時のユーザコードの扱い

ユーザの `*.sru` ファイル は **その場でパースして AST 直 walk** する。
バイトコードに毎回落とすこともできるが、組み込みイメージのために設けた
バイトコードを実行時にも使う旨味は薄い。よって:

- ユーザの実行は AST 評価器に流す。
- 組み込みイメージだけバイトコード経由で読み込む。
- 例外: `rsruc input.sru -o out.bc` で事前コンパイルされたファイルを実行
  したい場合は、`rsru run-bc out.bc` のようなサブコマンドを設ける。

## 7. CLI 概形

```
rsru                      # REPL or stdin eval (csru と同挙動)
rsru file.sru             # ファイル実行
rsru run-bc file.bc       # 事前コンパイル済バイトコード実行
rsruc input.sru -o file.bc [--const-fold]
```

`rsru` 本体は最小、`rsruc` は別バイナリにする (`src/bin/rsruc.rs`)。

## 8. テストの構成

| ディレクトリ | 内容 |
| --- | --- |
| `tests/parser/` | snapshot テスト |
| `tests/eval/` | 評価器単体テスト |
| `tests/samples/` | `csru/samples/` のスクリプトを実行、stdout を期待値と diff |
| `tests/bytecode/` | round-trip テスト、AST 直 walk と一致確認 |

CI:
- `cargo test --all`
- `cargo build --release` で `builtin.bc` が生成されることを確認
- 段階 A 完了時点では「`samples/` をパスする」を CI ゲートにする

## 9. 段階ごとのビルド構成変化

| 段階 | build.rs | sru_lib | 評価 |
| --- | --- | --- | --- |
| A | 無し（または空） | 無し | AST 直 walk のみ |
| B | 無し | あり、ただし起動時に文字列としてロード | AST 直 walk |
| C | あり、bc を埋め込む | あり | 起動時 bc → AST 直 walk |
| D | 同上 | 同上 | + native プラグイン |

段階 B のときは「ソースのまま埋め込む」(`include_str!`) でもよい。バイト
コード化は組み込みソースを **隠したい / 起動を速くしたい** 段階 C で
意味が出る。

## 10. 開発フローと既存コードとの関係

- `csru/` はリファレンス実装として残す。意味論の正解は常に csru の挙動。
- rsru 側でテストが落ちたら、まず csru で同じ入力を実行して期待値を確認。
- `memo/spec_*.md` の更新が必要になったら、csru の挙動を再確認したうえ
  で spec を直し、rsru を追従させる。コードに先んじて仕様を書き換えない。
