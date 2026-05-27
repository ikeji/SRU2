# rsru — Rust 版 SRU インタプリタ 設計概要

`csru/`（C++ 実装）の挙動を保ったまま、SRU を Rust に移植する。仕様は
`memo/spec_00_overview.md` 以下を真とし、本ドキュメント群は **どう作るか** だけを書く。

## 目標

1. `memo/spec_*.md` で記述した意味論を、別ホスト言語で再実装しても同じ
   `*.sru` プログラムが同じ結果を出すこと。
2. C++ 版の構造的に弱い点を是正する:
   - GC のルート集合とマークビットを `BasicObject` の外に出す。
   - 組み込みライブラリは可能な範囲で SRU で書く。
   - 組み込み SRU はバイトコードにコンパイルし、最終バイナリに静的同梱する。
3. 拡張のために `dlopen` 経由の native プラグインを呼べる余地を残す（ABI は
   csru と同形でなくてもよいが、目的を同じにする）。

## 非目標

- パフォーマンス最適化（インライン化・JIT 等）。
- 並行・スレッド対応。csru と同様に Single-thread 前提。
- 文法そのものの拡張。あくまで csru と同じ言語を実行する。

## 言語選定 (Rust)

- 参照カウント + マーク&スイープを `Rc<RefCell<…>>` でも、Arena +
  ハンドルでも書けて、安全側に倒せる。本設計では **arena + ハンドル**
  を採用する（`design_01_object_model.md`、`design_02_gc.md`）。
- AST が `enum` 一発で表現でき、トレース/評価が `match` に乗る。
- `extern "C"` で C ABI を素直に出せるので、native 拡張用の入口を作れる。
- ビルドスクリプト (`build.rs`) で `*.sru` をコンパイルして `include_bytes!`
  できる（`design_07_build.md`）。

## ドキュメント構成

| # | ファイル | 内容 |
| --- | --- | --- |
| 00 | `design_00_overview.md` | 本ドキュメント |
| 01 | `design_01_object_model.md` | `BasicObject` の Rust 表現、Value トレイト |
| 02 | `design_02_gc.md` | ルート集合とマークビットの外出し、Heap の API |
| 03 | `design_03_parser_ast.md` | AST 定義と手書きパーサの構成 |
| 04 | `design_04_evaluator.md` | スタックフレーム、Trace/Eval、継続 |
| 05 | `design_05_builtins.md` | 組み込みの Rust/SRU 分担、native プラグイン |
| 06 | `design_06_bytecode.md` | バイトコード形式、コンパイラ、`--const-fold` |
| 07 | `design_07_build.md` | クレート構成、`build.rs`、組み込みイメージ |

## 大まかな進め方

1. **段階 A — AST 直接評価**: パーサ + AST + Trace/Eval を Rust で書き、
   csru と同じテストを通す。バイトコードはまだ書かない。
2. **段階 B — 組み込みの SRU 化**: Rust ベタ書きの組み込みのうち、SRU で
   書けるものを `sru_lib/*.sru` に逃がす。AST 直接評価のままでよい。
3. **段階 C — バイトコード**: コンパイラと VM を実装し、`sru_lib/*.sru` を
   ビルド時にバイトコード化して `include_bytes!` で同梱する。
4. **段階 D — native FFI**: `requireNative` 相当を再導入する（必要なら）。

段階 A だけでも csru の代替として完結する想定。バイトコードはあくまで
組み込みライブラリを静的同梱するための手段として導入する。
