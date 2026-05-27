# rsru — パーサと AST

仕様: `memo/spec_01_lexical.md`、`memo/spec_02_grammar.md`。

## 1. AST 定義

仕様より AST ノードは 5 種類しかない:

```rust
pub enum Expression {
    Let     { var: SymbolId, env: Option<Box<Expression>>, value: Box<Expression> },
    Ref     { var: SymbolId, env: Option<Box<Expression>> },
    Call    { receiver: Option<Box<Expression>>, method: SymbolId, args: Vec<Expression> },
    Proc    { vargs: Vec<SymbolId>, retval: Option<SymbolId>, body: Vec<Expression> },
    StrLit  (String),
}
```

- リテラルは `StrLit` だけ。数値リテラルは `Numeric.parse("...")` の
  `Call` に展開される（仕様 §spec_02_grammar §4.5）。
- 真偽値・nil もパース時点では `Ref { var: sym("tlue") }` 等の `Ref` に
  なる（仕様 §spec_01_lexical §9）。
- `def foo(x) ... end` のような糖衣は **パース時に脱糖** して `Let` +
  `Proc` に変換する（仕様 §spec_02_grammar §6）。

`Expression` は `Clone` 可（`Proc::body` がコピーされて Frame に積まれる
ため）。`Box`/`Vec` を使うので Clone はディープになるが、頻度は高くない。

## 2. パーサ構成

csru のパーサは Ruby DSL (`parser_generator/sru_syntax.rb`) で生成された
手続き型コードだが、rsru では **手書きの recursive descent** にする。

理由:
- SRU の文法は演算子優先順位が固定の 12 段（仕様 §spec_02_grammar §3）。
  手書きで素直に書ける。
- `spc` と `spc_or_lf` の使い分け（カッコ内では改行が空白扱い、トップ
  レベルでは文区切り）が文脈依存。これを `nom` のコンビネータに乗せる
  と読みづらくなる。
- `class A < B` のようにキーワードでなく予約語経由の構文がある。手書き
  なら `eat_keyword("class")` で完結。

クレート構成:

```
src/parser/
  mod.rs       — pub fn parse(src: &str) -> Result<Vec<Expression>, ParseError>
  lexer.rs     — 字句要素（spc, ident, number, string, ...）
  expr.rs      — 式パーサ（precedence ladder）
  stmt.rs      — 文パーサ（if / while / def / class / etc.）
  desugar.rs   — def, class, postfix-if, array literal の展開
  error.rs     — ParseError と位置情報
```

## 3. レキサ層

仕様 `memo/spec_01_lexical.md` の各トークンを関数で持つ:

```rust
pub struct Lexer<'a> { src: &'a [u8], pos: usize }

impl<'a> Lexer<'a> {
    fn spc(&mut self);            // ' ', '\t'
    fn spc_or_comment(&mut self); // spc + '#' to before '\n'
    fn spc_or_lf(&mut self);      // spc + '\r', '\n' + '#' to '\n'
    fn lf(&mut self) -> bool;     // '\r\n' | '\r' | '\n'
    fn eos(&self) -> bool;
    fn ident(&mut self) -> Option<&str>;
    fn number(&mut self) -> Option<&str>;
    fn real(&mut self) -> Option<&str>;
    fn string_literal(&mut self) -> Option<String>; // " " で囲み、\n \r \\ \" 解釈
    fn try_eat(&mut self, kw: &str) -> bool;
    fn snapshot(&self) -> usize;
    fn restore(&mut self, p: usize);
}
```

- 失敗時は `restore` で位置を巻き戻す（PEG ライクな試行）。
- 識別子の `[a-zA-Z0-9_!]+` で `!=` を切る規則（仕様 §3）は `ident` の
  内側で実装。

## 4. 演算子優先順位

仕様 §spec_02_grammar §3 のテーブルをそのまま再現する。実装は標準的な
precedence climbing で、各レベルが次のような関数になる:

```rust
fn parse_expr_or  (&mut self) -> Result<Expression> { /* || */ }
fn parse_expr_and (&mut self) -> Result<Expression> { /* && */ }
fn parse_expr_eq  (&mut self) -> Result<Expression> { /* == === != > < >= <= */ }
fn parse_expr_bor (&mut self) -> Result<Expression> { /* | */ }
fn parse_expr_band(&mut self) -> Result<Expression> { /* & */ }
fn parse_expr_shift(&mut self)-> Result<Expression> { /* << >> */ }
fn parse_expr_add (&mut self) -> Result<Expression> { /* + - */ }
fn parse_expr_mul (&mut self) -> Result<Expression> { /* * / % */ }
fn parse_expr_unary(&mut self)-> Result<Expression> { /* ! ~ */ }
fn parse_expr_inst(&mut self) -> Result<Expression> { /* a.method (no args) */ }
fn parse_expr_call(&mut self) -> Result<Expression> { /* a.method(args) / a[i] / etc */ }
fn parse_expr_primary(&mut self) -> Result<Expression> { /* lit / ident / (...) / [...] / {...} */ }
```

各演算子は `a OP b` を `a.OP_METHOD(b)` の `Call` に脱糖する。マッピング表
は仕様 §spec_02_grammar §3.2。

## 5. 文と区切り

トップレベル文は `\n` または `;` または `eos` で区切られる（仕様 §spec_01
§8）。`parse_program` は次のループで構成:

```rust
fn parse_program(&mut self) -> Result<Vec<Expression>> {
    let mut out = vec![];
    loop {
        self.lex.spc_or_lf();
        if self.lex.eos() { break; }
        let e = self.parse_stmt()?;
        out.push(e);
        // 文末の区切り（lf / ';' / eos）を必ず消費
        if !self.consume_stmt_terminator() {
            return Err(self.error("expected statement terminator"));
        }
    }
    Ok(out)
}
```

- `if` / `while` / `def` / `class` などのブロック構文は内側で `lf` を許す。
- `class C ... end` 内、`{ |x| ... }` 内、`(...)` 内、`[...]` 内では
  `spc_or_lf` を消費する（改行が文区切りとして働かない）。

## 6. 脱糖

仕様 §spec_02_grammar §6 で挙げた糖衣はすべてパース後段の `desugar`
モジュールでこなす。AST には現れない:

- `def f(a, b) BODY end`
  → `Let { var: f, env: None, value: Proc { vargs: [a,b], retval: Some(sym("return")), body: BODY } }`
- `class C < Base ... end`
  → `Let { var: C, value: Call { receiver: Some(Ref Base), method: subclass, args: [StrLit "C"] } }`
  - 続けて class 本体内の `def` を `C.instanceMethods.foo = { ... }`
    相当の `Call(Let)` 群にする。詳しくは仕様 §6.4。
- `EXPR if COND`
  → `if COND then EXPR end`
- `[a, b, c]`
  → `Array.new` をベースに `push(a)` を畳む。または直接 Array リテラルに
    対応する組み込みコール（仕様 §spec_02_grammar §6.5）。
- `if/elsif/else`
  → `Boolean#if` を使った受け手チェーンに脱糖（仕様 §6.2）。
- `while COND ... end`
  → 自己呼び出しの Proc に脱糖（仕様 §6.3）。

## 7. エラー処理

`ParseError { msg, line, col, source_line }` を返す。`csru/main.cc` の
`PrintErrorPosition` と同じ情報を出せれば UX 上揃う。テストでは行・桁を
固定したい場合があるので `line`/`col` は明示フィールドで持つ。

## 8. テスト

`tests/parser/` 配下に snapshot テスト:

- `corpus/*.sru` を読み、AST を `Display` 実装でテキスト化して `*.txt`
  と比較。
- csru で動く既存サンプル（`csru/samples/`）を入力にし、AST 自体は
  比較しないが「パースが通る」事だけ確認する e2e テストも別途。
