# SRU 言語仕様 — 組み込みクラスとグローバル

ソース: `csru/library.cc`, `csru/object.cc`, `csru/class.cc`, `csru/nil.cc`,
`csru/boolean.cc`, `csru/numeric.cc`, `csru/sru_string.cc`, `csru/sru_array.cc`,
`csru/proc.cc`, `csru/binding.cc`, `csru/sru_math.cc`, `csru/sru_sys.cc`,
`csru/p.cc`, `csru/require.cc`, `csru/load.cc`, `csru/ast_class.cc`。

公開されている組み込み API を列挙する。実装されているが本体が `CHECK(false)`（未実装）のものはその旨を明記する。

## 1. グローバル束縛（`library.cc::BindPrimitiveObjects`）

ルート Binding に以下の識別子が設定される:

| 名前 | 中身 |
| --- | --- |
| `nil`     | Nil シングルトン |
| `Binding` | Binding クラス |
| `Class`   | Class クラス |
| `Object`  | Object クラス |
| `Proc`    | Proc クラス |
| `Array`   | Array クラス |
| `Hash`    | Hash クラス（プレースホルダ。`__name = "Hash"` のみ設定） |
| `String`  | String クラス |
| `Numeric` | Numeric クラス |
| `Boolean` | Boolean クラス |
| `tlue`    | True シングルトン（注: `true` ではなく `tlue`） |
| `farse`   | False シングルトン（注: `false` ではなく `farse`） |
| `__parser`| パーサオブジェクト |
| `Math`    | Math クラス |
| `Sys`     | Sys クラス |
| `AST`     | AST 名前空間 |

加えて、`p.cc` / `require.cc` / `load.cc` によって以下も登録される。

| 名前 | 説明 |
| --- | --- |
| `p`             | `obj.Inspect()` を出力して obj を返す |
| `puts`          | 文字列を表示し改行 |
| `print`         | 文字列を表示（改行なし） |
| `exit`          | `exit(int)` でプロセス終了 |
| `requireNative` | 共有ライブラリ (`.so`/`.dll`) を `dlopen` し、`require` シンボルを呼ぶ |
| `load`          | SRU ファイルを読み込んで評価 |
| `eval`          | AST を評価 |

> **注**: 真偽値の名前が `tlue` / `farse` なのはコード上の誤記がそのまま固定された状態。互換性のためにこの名前を残す（`csru/constants.cc` の `sym::tlue()`, `sym::farse()`）。実装者が「`true`/`false` で使いたい」と思った場合は、トップレベルに `true = tlue; false = farse` の SRU コードを追加でロードして対応する。

## 2. Object

`csru/object.cc::InitializeObjectClass`。
- 親: `Nil`
- インスタンスメソッド:
  - `equal(self, other)`    : ポインタ同一性
  - `notEqual(self, other)` : ポインタ非同一性

## 3. Class

`csru/class.cc`。
- 親: `Object`
- 自身の `klass` は `Class`（自己参照）
- 公開メソッド（インスタンスメソッド）:
  - `subclass(self [, name])` : 新しいサブクラスを生成して返す。第 2 引数を渡せば `__name` も設定。
  - `mew(self, args...)`       : new 相当。新インスタンスを作って `initialize(args...)` を呼ぶ。
    - SRU から書くときは `someClass.new(...)` と書く（`sym::mew()` は実装上のシンボル名）。
- 内部スロット:
  - `findInstanceMethod` / `findSlotMethod`: 検索手続きのテンプレート。クラス初期化時に各クラスへコピーされる。

## 4. Nil

`csru/nil.cc::SetupNilObject`。
- 単一インスタンス。`klass = Object`。`__name = "nil"`。
- 専用メソッドは無く、`Object` の `equal`/`notEqual` を引き継ぐ。

## 5. Boolean / True / False

`csru/boolean.cc`。
- `Boolean` クラスは `Object` の子。`__name = "Boolean"`。
- `True` / `False` は `Boolean` の単一インスタンス。直接スロットに次のメソッドを持つ:

| メソッド | True の挙動 | False の挙動 |
| --- | --- | --- |
| `ifTrue(self, block)`      | `block()` を呼ぶ | `nil` を返す |
| `ifFalse(self, block)`     | `nil` を返す | `block()` を呼ぶ |
| `ifTrueFalse(self, t, f)`  | `t()` を呼ぶ | `f()` を呼ぶ |
| `ampamp(self, other)`      | `other` を返す | `self` を返す |
| `pipepipe(self, other)`    | `self` を返す | `other` を返す |
| `exclamation(self)`        | `False` を返す | `True` を返す |
| `equal(self, other)`       | `other == True` で `True/False` | `other == False` で `True/False` |
| `notEqual(self, other)`    | 上記の否定 | 上記の否定 |

`if (cond) ... end` / `&&` / `||` / `!` の **すべて** がこれらメソッド呼び出しに展開される（短絡評価が成立する設計）。

## 6. Numeric

`csru/numeric.cc::InitializeClassObject`。
- 親: `Object`
- データは `int` または `double`（タグで分岐）。
- クラスメソッド:
  - `parse(self, str)` : 文字列を数値に変換。`"."` を含めば `double`, それ以外は `int`。
- インスタンスメソッド一覧:

| シンボル名 | 演算子 | 説明 |
| --- | --- | --- |
| `equal`           | `==`  | 数値等価。非数なら false。 |
| `notEqual`        | `!=`  | 否定。 |
| `greaterThan`     | `>`   | int/double 混在は double 昇格。 |
| `greaterOrEqual`  | `>=`  | 同上。 |
| `lessThan`        | `<`   | 同上。 |
| `lessOrEqual`     | `<=`  | 同上。 |
| `ltlt`            | `<<`  | int のみ。double 同士はエラー。 |
| `gtgt`            | `>>`  | int のみ。 |
| `plus`            | `+`   | 片方が double なら double。 |
| `minus`           | `-`   | 同上。 |
| `asterisk`        | `*`   | 同上。 |
| `slash`           | `/`   | int は切り捨て整数除算。 |
| `percent`         | `%`   | double は `fmod`、int は `%`。 |
| `invert`          | `-x`  | 単項マイナス。double 値も正しく扱う。 |
| `times(self, blk)`| —     | `self.times({|i| ... })`。0..self-1 を渡してブロックを呼ぶ。 |
| `toS(self)`       | —     | 文字列化。`12` → `"12"`, `1.5` → `"1.5"`。 |

Inspect 表示は `Numeric(N)` 形式。

## 7. String

`csru/sru_string.cc::InitializeStringClass`。
- 親: `Object`
- データは symbol（インターン文字列）。
- インスタンスメソッド:

| シンボル | 演算子 | 説明 |
| --- | --- | --- |
| `toS`              | —     | 自身を返す |
| `equal`            | `==`  | 文字列比較 |
| `notEqual`         | `!=`  |  |
| `lessThan`         | `<`   |  |
| `lessOrEqual`      | `<=`  |  |
| `greaterThan`      | `>`   |  |
| `greaterOrEqual`   | `>=`  |  |
| `get(self, i)`     | `[i]` | 1 文字取り出し（境界外は空文字列） |
| `substr(self, i [, n])` | — | 部分文字列。`n` 省略時は末尾まで |
| `size(self)`       | —     | バイト長を整数で返す |
| `plus(self, s)`    | `+`   | 連結 |

Inspect 表示は `String("...")` 形式。エスケープは `\n`, `\r`, `"`, `'` のみ。

## 8. Array

`csru/sru_array.cc::Array::InitializeClass`。
- 親: `Object`
- `Array.new` で空配列を生成。配列リテラル `[a, b, c]` も内部で `Array.new` を使用。
- データは `BasicObject*` の動的配列（`object_vector`）。

実装済みメソッド:
| シンボル | 別名 | 内容 |
| --- | --- | --- |
| `get(self, i)`     | `at`, `slice`, `[]` | 添字アクセス。負の値は末尾基準。境界外は nil |
| `set(self, i, v)`  | `[]=` | 添字代入。サイズ不足時は nil で埋める |
| `push(self, v...)` | `ltlt` (`<<`) | 末尾追加（可変長） |
| `clear(self)`      | — | 全要素削除 |
| `concat(self, *arrays)` | — | 末尾に複数配列を連結 |
| `each(self, block)`| `eachIndex` | 各要素にブロック適用 |
| `replace(self, other)` | — | 他配列で中身を置き換え |
| `reverse(self)`    | — | 反転した新配列を返す |
| `reverseEx(self)`  | — | self を破壊的に反転 |
| `size(self)`       | — | 長さ |
| `toA(self)`        | — | 自身を返す |

宣言だけされていて本体未実装（呼ぶと CHECK failure）:
`amp`, `asterisk`, `plus`, `minus`, `pipe`, `assoc`, `choice`, `collectEx`, `combination`,
`compact`, `compactEx`, `derete` (=`delete`), `dereteIf` (=`deleteIf`), `emptyQ`, `fetch`,
`fill`, `flatten`, `flattenEx`, `index`, `insert`, `join`, `last`, `length`, `mapEx`,
`nitems`, `pack`, `permutation`, `pop`, `product`, `rassoc`, `rejectEx`, `rindex`,
`shift`, `shuffle`, `shuffleEx`, `sliceEx`, `sortEx`, `transpose`, `uniq`, `uniqEx`,
`unshift`, `valuesAt`。

> 未実装メソッドは「同じ名前のスタブが存在するが本体は `CHECK(false)`」という状態。シンボルとしては検索可能だが呼ぶと落ちる。

Inspect 表示は `Array size=N [e1, e2, ...]`。

## 9. Hash

`library.cc:172` で `__name = "Hash"` を設定するだけのプレースホルダ。実装は無い。

## 10. Proc

`csru/proc.cc`。
- 親: `Object` (実装上 `SetAsSubclass(proc, NULL)` だが内部で Object 扱い)
- ネイティブ手続き (`NativeProc`, `NativeProcWithSmash`) と SRU ユーザ定義 (`SRUProc`) を共通インタフェースで扱う。
- インスタンスメソッド:
  - `whileTrue(cond_block, body_block)` :
    `body_block` を `cond_block()` が真を返す間繰り返す。専用バインディング上で `_whileTrue_internal` を回す形で実装。
  - `loop(block)` : 無限ループ。継続で抜け出す前提。
  - `apply(proc, args_array)` : 配列を展開して呼ぶ（`f.apply([1,2,3])` ≡ `f(1,2,3)`）。
  - `ylppa(proc)` : `apply` の逆。引数を配列にまとめて proc に渡すラッパを返す（`f.ylppa()(1,2,3)` ≡ `f([1,2,3])`）。

呼び出しシグネチャ:
- ネイティブ Proc: `Call(context, proc, args)` を実装。
- SRU Proc: `vargs`, `retval`, `expressions[]`, `binding` を持ち、`Call` で `Binding::New(binding)` → `DigIntoNewFrame` → 引数束縛 → 継続束縛 を行う。

## 11. Binding

`csru/binding.cc`。
- 親: `Class`（インスタンス側は通常の Object）。
- 生成は `Binding::New([parent])`。
- スロット: `local`, `_parent`（任意）, `__findSlot`, `get`, `set`。
- 公開メソッド:
  - `get(env, name)` : ローカルスロットを返す。無ければ nil。
  - `set(env, name, val)` : ローカルスロットに代入。
  - 内部の `__findSlot` で親 Binding チェーンを辿るのは `spec_03_semantics.md` 参照。

## 12. Math

`csru/sru_math.cc`。
- 親: `Object`
- メソッド（クラスメソッドとして直接スロットに設定）:
  - `sin(_, x)` / `cos(_, x)` : double を返す
  - `srand(_ [, seed])` : 乱数シード設定。省略時は `time(NULL)`
  - `rand(_ [, max])`   : `max` 指定時は `0..max-1` の整数、省略時は `[0,1)` の double

## 13. Sys

`csru/sru_sys.cc`。
- 親: `Object`
- メソッド: `clock(_)` : `clock()/CLOCKS_PER_SEC` を double で返す。

## 14. Parser (`__parser`)

`csru/parser_*.cc` 全体で構成されるパーサオブジェクト。
公開 API は `parse(src)` のみ。返り値は `{ ast, pos, error, status, src }` のスロットを持つ BasicObject（`spec_02_grammar.md` 参照）。

## 15. AST 名前空間

`csru/ast_class.cc`。SRU 側から AST を構築するためのクラス群:
- `AST.LetExpression`, `AST.RefExpression`, `AST.CallExpression`,
  `AST.ProcExpression`, `AST.StringExpression`。

それぞれ `new(...)` でインスタンスを作るとパーサが組むのと同じ AST オブジェクトが得られる。マクロ／メタプログラミングや、独自パーサの結果を返すための拡張口。

## 16. グローバル関数のシグネチャまとめ

| 関数 | 引数 | 戻り値 | 備考 |
| --- | --- | --- | --- |
| `p(x)`               | 任意           | `x` | `x.Inspect()` を改行付きで標準出力 |
| `puts(s)`            | String         | `s` | 改行付き出力 |
| `print(s)`           | String         | `s` | 改行なし出力 |
| `exit(n)`            | Numeric        | （戻らない） | `exit(n)` を呼ぶ |
| `requireNative(name)`| String         | True/False | `.so`/`.dll` を `dlopen` し、`require(env)` を呼ぶ |
| `load(filename)`     | String         | True/False | SRU ファイルを読み込んで評価 |
| `eval(ast)`          | AST ノード     | ast 評価結果 | 現在の Binding 上で評価 |

> 真偽値の名前は `tlue` / `farse` だが、`True` クラスシングルトン / `False` クラスシングルトン のオブジェクト ID で同一性比較される。

## 17. 既知の整合性課題

- `Hash` クラス相当の実装が無い。リテラル `{}` はクロージャと衝突するため、ハッシュリテラルは無い。
- `String` には書式整形 (`%`) や split 等が無い。
- `nil` から `puts`/`print` を呼ぶと `SRUString::GetValue` が空文字列を返す（クラッシュはしない）。
- `requireNative` の呼び出し先 (.so) は `bool require(const BasicObjectPtr&)` を export する必要がある（`csru/lib/ffi.cc` などが例）。
