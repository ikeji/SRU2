# SRU 言語仕様 — ランタイム動作

ソース: `csru/main.cc`, `csru/interpreter.cc`, `csru/load.cc`, `csru/require.cc`,
`csru/object_pool.cc`。

## 1. 起動形態

`csru/main.cc::main` より、コマンドラインの引数の有無で挙動が変わる。

| 起動 | 挙動 |
| --- | --- |
| `sru` （引数なし、TTY） | REPL を起動（`USE_EDITLINE` が有効なら editline 付き、なければ素の `cin`） |
| `sru` （引数なし、パイプ入力） | 標準入力を全部読み込んで一括評価 |
| `sru filename.sru` | ファイルを読み込んで一括評価 |

REPL では:
- プロンプトは `>> `。
- 1 行入力ごとに `Interpreter::Eval(line)` を呼ぶ。
- 評価結果が non-NULL なら `_1`, `_2`, ... という名前と `_` で結果を保存し、`_N = <Inspect>` で表示。
- 空行で終了。
- editline 有効時はタブ補完が効く（symbol テーブルに登録された名前を補完候補に出す）。

## 2. インタプリタの永続状態

`Interpreter` は Singleton。`Interpreter::Instance()` で取得。
- `current_frame` : 現在のスタックフレーム
- `root_frame`    : ルートのスタックフレーム
- 初期化時 (`InitializeInterpreter`) に
  1. `StackFrame::New(Binding::New())` を作って両方に格納
  2. `Library::BindPrimitiveObjects(binding)` で組み込み定数を束縛
  3. `SetupRequireFunction(binding)` / `SetupLoadFunction(binding)` / `SetupPrintFunction(binding)` でグローバル関数を束縛

## 3. 評価ループ（一括実行）

`csru/main.cc::evalStream`:
```cpp
string prog = (ストリームを全部読む);
BasicObjectPtr r = Interpreter::Instance()->Eval(prog);
if (r != NULL) LOG << r->Inspect();
```

`Interpreter::Eval(string)`:
1. `__parser.parse(src)` を AST 構築のために評価。
2. 戻り `obj` の `ast` を取り出し、`Eval(ast)` で実行。
3. `obj.pos` が `src.size()` に到達するか、残りが空白のみになるまでループ。
4. 各反復で残ソースを更新（`src = src.substr(pos)`）。

そのため「複数文をまとめて投げる」場合は、各文ごとに parser を呼んで AST を 1 個ずつ評価していく方式。

## 4. REPL での失敗時挙動

- パースに失敗すると `Eval(string)` は NULL を返す。
  - `LOG_ALWAYS << "Parse error: " << err`
  - `PrintErrorPosition(src, pos)` でエラー位置を表示（行と桁、当該行のソース）。
- 実行時 `CHECK(false)` が走るとプロセス自体が abort する（例外ハンドリングなし）。

## 5. `load(filename)`

`csru/load.cc::Load`:
- 引数文字列のファイルを開き、全文を `prog` に読み込み、`Interpreter::Eval(prog)` を呼ぶ。
- 結果が non-NULL なら True、NULL なら False を返す。
- 注意: コード内コメントに「This code is invalid. Eval is not re-entrant.」とある。再帰的な `load` は厳密には未対応。

## 6. `eval(ast)`

`csru/load.cc::Eval`（`DEFINE_SRU_PROC_SMASH`）:
- 引数 1 個（AST ノード）を受け取り、現在のフレームの上に `DigIntoNewFrame` を積んで評価する。
- AST を生成して `eval(ast)` するパスはマクロや動的コード生成で使う。

## 7. `requireNative(libname)`

`csru/require.cc`:
- `dlopen(libname, RTLD_NOW)` で共有ライブラリを開く。
- そこから `require` シンボル（`bool require(const BasicObjectPtr& env)`）を取得して呼ぶ。
- `require` 関数の中で `env` (現在の Binding) にメソッドを追加するのが拡張作法。
- 戻り値 True/False は `require` 関数が成功したかどうか。
- Windows では `LoadLibrary`/`GetProcAddress` を使う薄い互換層あり。

## 8. ファイルの探索

- `load` も `requireNative` も **カレントディレクトリ起点** で素朴にパスを解釈する。
- SRU 自身に `$LOAD_PATH` 相当の機構は無い。

## 9. 標準出力・エラー出力

- `p`, `puts`, `print` はすべて `std::cout`。stdout フラッシュは行末でしか保証されない。
- ロギング (`LOG_*`) は `stderr`。デバッグビルドでないと出ない。

## 10. シンボルテーブル

`csru/symbol.{h,cc}`:
- 文字列 → 一意な整数 ID を割り当てるシングルトンテーブル（`hash_map<string, int>`）。
- スロットアクセスはすべて整数 ID で行うので O(1)。
- 既知シンボルはコード生成された `constants.cc` で事前にアクセサ `sym::xxx()` が定義されている（150 個以上）。

## 11. オブジェクト管理

`csru/object_pool.cc`:
- すべての `BasicObject::New` はプールに登録される。
- `BasicObjectPtr` の参照カウントが 0 になったオブジェクトを GC 機会で `Mark` 走査 → `Sweep`。
