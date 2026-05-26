# SRU 言語仕様 — 構文

ソース: `csru/parser_generator/sru_syntax.rb`, `csru/parser_statements.cc`,
`csru/parser_expression.cc`, `csru/parser_closure.cc`, `csru/parser_call.cc`,
`csru/parser_method_call.cc`, `csru/parser_class.cc`, `csru/parser_def.cc`,
`csru/parser_let.cc`, `csru/parser_if.cc`, `csru/parser_while.cc`,
`csru/parser_program.cc`。

`csru/memo/syntax.txt` の BNF を基に、コード内の追加ルール（後置 `if`、`%` 演算子、配列リテラル等）を統合した。

## 0. 表記

- `A | B` … 選択
- `[A]`   … 任意（0 or 1）
- `{A}`   … 0 回以上の繰返し
- 終端文字列は `"..."` で囲む。
- `spc`, `spc_or_lf`, `spc_or_comment`, `lf`, `eos` は `spec_01_lexical.md` を参照。

## 1. プログラム全体

```
program          ::= after_if_statement
                     { spc_or_comment ( lf | ";" | eos ) after_if_statement }
                     [ spc_or_comment ( lf | ";" | eos ) ]
```

トップレベルでは複数文を改行・`;`・EOF で区切って並べる（`csru/parser_program.cc`）。

## 2. 文の構造

```
statements           ::= { spc_or_lf
                            ¬"end" ¬"else" ¬"elsif"
                            after_if_statement
                            spc_or_comment ( lf | ";" | eos ) }
after_if_statement   ::= statement [ spc "if" after_if_statement ]
statement            ::= spc_or_lf ( let_statement | flow_statement )
let_statement        ::= flow_statement spc "=" spc_or_lf statement
flow_statement       ::= if_statement
                       | while_statement
                       | class_statement
                       | def_statement
                       | expression
```

ポイント:
- **後置 if**: `statement if cond` は `cond.ifTrue({ statement })` に等価な AST に展開される（`parser_statements.cc` で `after_if_statement` を組み立てる）。
- **let_statement** の左辺は `flow_statement` を再利用するが、最終的に `RefExpression` を返すものに限定される（識別子参照、または `obj.method` の参照形）。
- **代入演算子は `=` のみ**。`+=`, `-=` 等は無い。

## 3. 制御構文

### if
```
if_statement ::= "if" if_main
if_main      ::= spc_or_lf "(" spc_or_lf after_if_statement spc_or_lf ")" spc_or_comment ( lf | "then" | ";" )
                 [ statements ] spc_or_lf
                 ( "elsif" if_main
                 | "else" statements spc_or_lf "end"
                 | "end" )
```

- 条件式は `(` `)` で囲む（必須）。
- 条件直後は改行 / `;` / `then` のいずれかで本体に進む。
- `elsif` は何段でも続けられる。`else` 省略可。閉じは `end`。

### while
```
while_statement ::= "while" spc_or_lf "(" spc_or_lf after_if_statement spc_or_lf ")"
                    spc_or_lf statements spc_or_lf
                    spc_or_lf "end"
```

- C ライクに条件は `(` `)` 必須、本体は `end` で閉じる。
- `do while` / `loop` 単体構文は無い（`Proc#loop` メソッドで代替）。

### class
```
class_statement ::= "class" spc_or_lf ident
                    [ spc_or_lf "<" spc_or_lf after_if_statement ]
                    { spc_or_lf
                      "def" spc_or_lf ident spc_or_lf
                      "(" spc_or_lf [ ident { spc_or_lf "," spc_or_lf ident } ] ")"
                      spc_or_lf statements spc_or_lf "end" }
                    spc_or_lf "end"
```

- 親クラス指定は `<` （メモ `syntax.txt` 内の `:` ではなく実装は `<`）。
- 親を省略すると `Object` が暗黙の親となる（後述）。
- `class` の本体内に書けるのは `def メソッド` だけで、トップレベル文や代入は書けない。
- `sru_syntax.rb:81-97` のコメント通り、AST 上は以下のクロージャに糖衣展開される:

```
A = {
  $$ = ParentClass.subclass()
  $$.instanceMethods.method1 = {|self:return| ... }
  $$.instanceMethods.method2 = {|self, x:return| ... }
  $$
}()
```

`$$` は重複しにくい内部シンボル名で、外部から名前衝突しない（`sym::doldol` 等）。

### def
```
def_statement ::= "def" spc_or_lf ident
                  "(" spc_or_lf [ ident { spc_or_lf "," spc_or_lf ident } ] ")"
                  spc_or_lf statements spc_or_lf "end"
```

- トップレベルや `class` 外の任意の場所で書ける。
- 糖衣展開: `def f(a, b) ... end`  →  `f = {|a, b:return| ... }`
- 戻り値継続変数 `return` が必ず仕込まれる（`spec_03_semantics.md` 参照）。

### return / break 等
- 専用構文は無い。`def`/`class` 内で自動的に束縛される `return` 識別子を呼び出すと、その関数の呼び出し位置に戻る継続が起動される。
- `break` / `continue` キーワードも無い。ループ脱出は同様に継続を捕まえて行う。

## 4. 式の階層と演算子優先順位

優先順位が低い順に並べる（`sru_syntax.rb:142` 以降）。
すべて左結合。

| レベル | 名前 | 受理する演算子 | 説明 |
| ---: | --- | --- | --- |
| 1 | `expression` | `\|\|` | 論理 OR、短絡 |
| 2 | `bool_term`  | `&&` | 論理 AND、短絡 |
| 3 | `comp`       | `==`, `===`, `!=`, `>`, `<`, `>=`, `<=` | 比較。`===` は別ノード扱いだが実装は等価判定 |
| 4 | `bit_sim`    | `\|`            | ビット OR |
| 5 | `bit_term`   | `&`             | ビット AND |
| 6 | `bit_shift`  | `<<`, `>>`      | ビットシフト |
| 7 | `sim`        | `+`, `-`        | 加減算 |
| 8 | `term`       | `*`, `/`, `%`   | 乗除剰余 |
| 9 | `factor`     | `!`, `~` (前置) | 否定、ビット反転 |
| 10 | `instance_method` | `.メソッド(...)`, `.メソッド` 等 | メソッド呼び出し（ドット記法） |
| 11 | `method_call`     | `f(...)`, `f arg1, arg2`, `f[i]` | 関数呼び出し・添字 |
| 12 | `primary`         | `(式)`, `- primary`, リテラル, 参照 | 終端式 |

各二項演算子は対応するメソッド呼び出しに展開される。例えば `a + b` は `a.plus(b)` 相当の `CallExpression`、`a == b` は `a.equal(b)` 相当の AST になる。詳細メソッド名は `spec_04_builtins.md` を参照。

`%` も term レベル（実装 `sru_syntax.rb:200-207` および `csru/numeric.cc::Percent`）。

## 5. メソッド呼び出しと参照

### `method_call`（先頭プライマリへの呼び出し）

```
method_call ::= primary
                { spc "(" args ")"
                | spc ident_args            # 空白引数: f a, b, c
                | spc "[" expression "]" }  # 添字
```

- `f(a, b)` … カッコ付き呼び出し。0 引数も `()` を書く。
- `f a, b`  … 空白＋カンマ区切り（後置 if との衝突を避けるため、引数列の先頭で `if` を許さない先読み除外あり）。
- `f[i]`    … `f.get(i)` 相当（後述）。

### `instance_method`（ドット記法）

```
instance_method ::= [ method_call ]
                    { "." ident
                      ( "(" args ")"
                      | ident_args
                      | (参照のみ) )
                      { "(" args ")" | "[" expression "]" } }
```

- 連鎖呼び出し: `a.b().c().d`。
- インデックス: `a.b[i]` は「`a.b`」を取り出してから `[i]` で `get` を呼ぶ。
- メソッド呼び出しと参照を文法上は別ノードとして区別している。

## 6. 一次式（primary）

```
primary       ::= primitive
                | spc "-" primary             # 単項マイナス → invert
                | spc "(" spc_or_lf after_if_statement spc_or_lf ")"
primitive     ::= reference | literal
reference     ::= ident
literal       ::= closure_literal | const_literal | array_literal
const_literal ::= real | number | const_string
```

### closure_literal

```
closure_literal ::= spc "{" closure_varg statements spc_or_lf "}"
closure_varg    ::= spc [ "|" ]
                    [ spc_or_lf ident { spc_or_lf "," spc_or_lf ident } ]
                    [ ":" ident ]
                    spc_or_lf "|"
```

- 引数列が無いブロックは `{ ... }`。中身が `|` で始まらないなら varg 省略形 (`{| | ...}` と等価)。
- 仮引数列の終わりにオプションで `:retName` を書くと、その名前で **現在のスタックフレームを脱出する継続** が束縛される。`def f(...) ... end` は `:return` を必ず付ける糖衣として展開される。

### array_literal

```
array_literal ::= "[" spc_or_lf
                  [ after_if_statement
                    { spc_or_lf "," after_if_statement }
                    [ "," ] ]
                  spc_or_lf "]"
```

- 末尾カンマ可。
- AST 上は `$$ = Array.new(); $$.push(e1); $$.push(e2); ...; $$` のクロージャに展開される（`parser_constants.cc:136-176`）。

## 7. AST ノード（実装上の 5 種）

すべての構文要素は最終的に以下 5 種類の `Expression` に落とし込まれる（`csru/ast.h`）。

| ノード | フィールド | 用途 |
| --- | --- | --- |
| `RefExpression`    | `env`, `name` | 識別子参照。`env` が nil なら現在のバインディング、そうでなければ `env` を評価してそのオブジェクトのスロットを引く。 |
| `LetExpression`    | `env`, `name`, `rightvalue` | 代入。スコープ探索ルールは `spec_03_semantics.md` 参照。 |
| `CallExpression`   | `proc`, `arg[]` | 手続き呼び出し（メソッド呼び出しも含む）。 |
| `ProcExpression`   | `varg[]`, `retval`, `expressions[]` | クロージャ定義。`retval` が空でなければ継続を束縛する。 |
| `StringExpression` | `string` (symbol) | 文字列リテラル。 |

数値リテラルや配列リテラルは、上記 5 種のノードの組み合わせ（`CallExpression`/`StringExpression`/`RefExpression`/`ProcExpression`）として表現される。専用の数値ノードは存在しない。

## 8. パーサ API（実装側の契約）

`csru/interpreter.cc:130-169` より:

- グローバルバインディングに `__parser` というオブジェクトがあり、その `parse(src)` が AST を返す。
- `parse` の返値オブジェクトのスロット:
  - `ast` … 成功時の AST、失敗時は nil
  - `pos` … 次にパースを再開する位置（整数）
  - `error`/`src` … 失敗時のエラーメッセージとソース
- `Interpreter::Eval(string)` は `parse` → `Eval(ast)` を `pos == size` になるまでループする。

## 9. 文末・空白の細則

- 多くの規則で `spc_or_lf` を許しているため、演算子の前後・カッコの内側で改行できる。
  ```
  1 +
    2
  ```
  はパース可能。一方で
  ```
  1
  + 2
  ```
  は `1` の文で切れ、続く `+ 2` は単項プラスとして解釈されない（`+` は単項としては定義されていない）ためエラー。
- `method_call` の「空白引数呼び出し」`f a, b` は、引数開始の先頭に来てはいけないトークンとして
  `[` `.` `if` `end` `elsif` `then` `else` `lf` `|` `&` `=` `!` `>` `<` `+` `-` `*` `/` `%` `~` `#` を除外している（衝突回避）。
- `class` ブロックや `def` ブロックの本体は `statements` を使うので、文と文の間に任意個の空白行・コメントを置ける。

## 10. 例

`csru/screen.sru` から:
```sru
requireNative "lib/ffi.so"

def initscr()
  ffi_call("libncursesw.so.5", "initscr", "pointer", [], [])
end

initscr(); cbreak(); noecho();
move(1,1)
addstr("hello")
refresh()
```

`csru/a.sru`（継続のサンプル）:
```sru
{ def a() return(return) end; def b() x = a(); p 3; x() end; b()}()
```
- `def a()` の中で `return` を呼び戻り値として返す ⇒ `b()` 側で `x` に「`a()` の戻り先継続」が入る。
- `b()` の後半で `x()` を呼ぶと `a()` の呼び出し直後まで巻き戻り、もう一度 `b()` を続行する。
- 外側 `{ ... }` は無名クロージャの即時実行（末尾 `()`）。
