# rsru — 組み込みライブラリ

仕様: `memo/spec_04_builtins.md`、`csru/library.cc`、`csru/numeric.cc` 他。

## 1. 方針

csru は組み込みメソッドのほとんどを C++ で書いている。一方で、よく見ると
**SRU 自身でも書ける** ものが多い:

- `Numeric#abs` は `self < 0 ? self.neg : self`。Numeric の比較・unary
  minus さえ Rust にあれば SRU で書ける。
- `Array#each` は `i = 0; while i < self.length ... end`。length, get, +,
  < があれば SRU。
- `Boolean#if` は `self.then(thenProc, elseProc)` の派生で組める。

ポリシー:

1. **Rust 側に置くのは「データに直接触る」プリミティブだけ**。例: 整数の
   `plus`、文字列の `length`、ファイル入出力、`Numeric.parse`、低レベル
   I/O (`p`/`puts`)、`Sys.clock` 等。
2. それ以外（コレクションの高階メソッド、Class の subclass、Boolean の
   `if`/`then`/`else` 連鎖、Object の `inspect` のディスパッチなど）は
   `sru_lib/*.sru` に置き、ビルド時にバイトコードへ落とす（段階 C）。
3. 段階 A では「全部 Rust ベタ書き」でかまわない。段階 B で段階的に SRU
   に逃がす。

## 2. クラスとプリミティブの責務

`memo/spec_04_builtins.md` のクラス別に整理:

### Class

| メソッド | 実装 | 備考 |
| --- | --- | --- |
| `subclass(name)` | Rust | 新しい Class オブジェクトを作る。klass/superclass/instanceMethods をセット |
| `new(args...)` | Rust | インスタンス作成、`initialize` 呼び出し |
| `instanceMethods` | スロット | 直接 fields に置く |

### Object

| メソッド | 実装 |
| --- | --- |
| `inspect` | SRU でディスパッチ、データに触る末端は Rust |
| `equal(other)` | Rust（同一性比較） |
| `notEqual(other)` | SRU (`!self.equal(other)`) |

### Nil

| メソッド | 実装 |
| --- | --- |
| `inspect` | Rust（リテラル文字列 `"nil"`） |
| Boolean 系 | SRU で `tlue`/`farse` に振り分け |

### Boolean (`tlue` / `farse`)

| メソッド | 実装 |
| --- | --- |
| `if`, `unless`, `and`, `or`, `not` | SRU |
| `then(p)`, `else(p)` | SRU |
| 内部の分岐実体 | Rust (`true_branch`, `false_branch` の最小 2 メソッド) |

`tlue`/`farse` の綴りは仕様どおり保つ（`memo/spec_04_builtins.md
§Boolean`）。

### Numeric

| メソッド | 実装 |
| --- | --- |
| `plus`, `minus`, `multiply`, `divide`, `mod` | Rust |
| `equal`, `less`, `greater`, `lessEq`, `greaterEq` | Rust |
| `parse(str)` | Rust |
| `abs`, `negate`, `times(p)` | SRU |
| `to_s` | Rust |

### String

| メソッド | 実装 |
| --- | --- |
| `plus`, `equal`, `length` | Rust |
| `at(i)`, `slice(s,e)` | Rust |
| `to_s` | Rust（self を返す） |
| `inspect` | Rust（`EscapeString` 相当） |
| 高度な操作（split など、csru で stub になっているもの） | SRU か未実装のままにする |

### Array

| メソッド | 実装 |
| --- | --- |
| `new`, `length`, `at(i)`, `setAt(i,v)`, `push(v)` | Rust |
| `each`, `map`, `select`, `inject` | SRU |
| csru で `CHECK(false)` 止まりのもの | 段階 A では未実装 stub、段階 B で SRU 実装 |

### Hash

csru ではほぼ未実装（`__name="Hash"` のみ）。rsru でも段階 A はそれに
ならう。後で SRU 実装する。

### Proc / Binding

| メソッド | 実装 |
| --- | --- |
| `Proc#call(args)` | Rust（評価器に直結） |
| `Binding#get(name)`, `Binding#set(name, val)` | Rust |

### Math, Sys

| メソッド | 実装 |
| --- | --- |
| `Math.sin`, `cos`, `sqrt`, `srand`, `rand` | Rust |
| `Sys.clock` | Rust |

### グローバル

| 名前 | 実装 |
| --- | --- |
| `p`, `puts`, `print` | Rust |
| `exit` | Rust |
| `load(filename)` | Rust |
| `eval(ast)` | Rust |
| `requireNative(libname)` | Rust（段階 D） |
| `tlue`, `farse`, `nil` | Rust（singleton 束縛） |

## 3. ブートストラップ順序

```
1. Heap::new()
2. シンボルをプリインターン
3. Class オブジェクトを生成（klass/superclass は仮セット）
4. Object/Nil/Boolean/.../Sys を順に作る
5. 4 の最後で Class の klass/superclass を真の値に直す（自己参照）
6. tlue, farse, nil の singleton instance を作って root binding に束縛
7. 各クラスの instanceMethods に Rust 実装の Native proc を挿入
8. （段階 B 以降）組み込み image をロードして、SRU 製メソッドを上書き
```

ステップ 8 のロードは `eval` を内部で呼んで `*.sru` 相当を実行する。
段階 C ではバイトコードを直接 VM に流し込む（`design_06_bytecode.md`）。

## 4. Native プラグイン (`requireNative`)

仕様 `memo/spec_05_runtime.md §7`。

```rust
// 拡張ライブラリ側の C ABI
#[no_mangle]
pub extern "C" fn sru_require(env: u32, ctx: *mut SruContext) -> bool { ... }
```

- 段階 A/B では実装しない。
- 段階 D で実装する場合、`SruContext` には Heap のハンドル取得 API、
  シンボル取得 API、ObjData 作成 API を C ABI 関数ポインタで持たせる。
  そうすればプラグインクレートを `cdylib` でビルドして配布できる。
- csru 互換 ABI（`bool require(const BasicObjectPtr& env)`）は C++ 名前
  装飾と `BasicObjectPtr` の型が露出するので、互換性は **取らない**。
  rsru は新規 ABI を出し、csru プラグインの移植は再ビルド前提とする。

## 5. `sru_lib/` の構成案

段階 B 以降、`rsru/sru_lib/` に SRU で書く組み込みを置く:

```
sru_lib/
  bool.sru      # Boolean#if, unless, and, or, not, ...
  numeric.sru   # Numeric#abs, negate, times, ...
  string.sru    # String#each_char, etc.
  array.sru     # Array#each, map, select, inject, ...
  object.sru    # Object#inspect dispatch
  prelude.sru   # 最後にロードされ、全部を取りまとめる
```

ロード順は `prelude.sru` の中で `load("...")` を並べることで明示する。
バイトコード化後はこの順序を build.rs が読み取り、結合バイナリにまとめる
（`design_07_build.md`）。

## 6. テスト

- 各組み込みメソッドの単体テスト（Rust 側のユニットテスト + SRU 側で
  `tests/sru/builtin_*.sru`）。
- csru の `samples/*.sru` を一通り通すリグレッションテスト。
- 同一スクリプトを csru と rsru で実行し、stdout を diff する diff-test。
