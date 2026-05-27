# rsru — 評価器

仕様: `memo/spec_03_semantics.md §3-§5`、`memo/scheme-like.txt`、
`csru/stack_frame.cc`、`csru/proc.cc`。

## 1. 評価モデル

csru は **Trace + Eval の 2 段** で動く。木の評価をそのまま再帰呼び出しに
すると C スタックを使い切るので、

1. `TraceVisitor` が AST をポストオーダーに平坦化して `operations`（線形
   配列）を作る。
2. `EvalVisitor` が `operations` を 1 ステップずつ取り出し、値スタック
   `local_stack` を操作する。

rsru もこれを踏襲する。CPS や AST 直 walk もアイデアとしてはあるが、継続
の実装が一気に難しくなるので、まず csru 互換のモデルで作る。

## 2. StackFrame

```rust
pub struct StackFrame {
    pub expressions: Vec<Expression>,   // これから評価する文の列
    pub tree_it:     usize,             // expressions の次インデックス
    pub operations:  Vec<TraceOp>,      // 平坦化済みステップ
    pub it:          usize,             // operations の次インデックス
    pub local_stack: Vec<ObjId>,        // 値スタック
    pub binding:     ObjId,             // 現在の Binding オブジェクト
    pub upper:       Option<Box<StackFrame>>, // 呼び出し元（連結リスト）
}
```

- `upper` を `Box<StackFrame>` にすることで、フレームコピーが「ルート
  まで全部辿ってコピー」にならない（連結リストの**先頭 1 個だけ**
  コピーすれば csru と等価）。
- `StackFrame` 自体は `ObjData::Frame` として `Heap` 上の `BasicObject`
  に格納される。継続のためにヒープ確保が必要。

## 3. TraceOp

仕様の 5 AST に対応するステップ + 制御命令:

```rust
pub enum TraceOp {
    PushStr(String),               // StringExpression
    LookupRef { var: SymbolId, has_env: bool },
                                    // RefExpression: has_env ? pop env else env=binding
    Assign    { var: SymbolId, has_env: bool },
                                    // LetExpression
    Call      { method: SymbolId, argc: usize, has_recv: bool },
                                    // CallExpression
    MakeProc  { vargs: Vec<SymbolId>, retval: Option<SymbolId>, body: Vec<Expression> },
                                    // ProcExpression
    Pop,                            // 直前の値を捨てる（文の最後で使う）
}
```

- `MakeProc.body` は AST の所有を持つ。Trace 時点では「中身」をさらに
  平坦化はしない（Proc が呼ばれた時に新フレームで Trace する）。

## 4. Trace 手順

`fn trace(expr: &Expression, out: &mut Vec<TraceOp>)`:

- `StrLit(s)` → `PushStr(s)`
- `Ref { var, env: None }` → `LookupRef{ var, has_env: false }`
- `Ref { var, env: Some(e) }` → `trace(e); LookupRef{ var, has_env: true }`
- `Let { var, env, value }` → `[env を trace,] trace(value); Assign{var, has_env}`
- `Call { receiver, method, args }`:
  ```
  receiver があれば trace(receiver)
  args を順に trace
  Call { method, argc: args.len(), has_recv: receiver.is_some() }
  ```
- `Proc { vargs, retval, body }` → `MakeProc{...}`

トップレベルの文は `[trace(stmt); Pop]` で連結（最後の文の `Pop` は省略
してもよい）。

## 5. Eval 1 ステップ

```rust
pub fn step(heap: &mut Heap, frame: &mut StackFrame) -> StepResult {
    if frame.it < frame.operations.len() {
        let op = frame.operations[frame.it].clone();
        frame.it += 1;
        match op {
            TraceOp::PushStr(s)       => frame.local_stack.push(heap.alloc_str(&s)),
            TraceOp::LookupRef { var, has_env } => do_ref(heap, frame, var, has_env),
            TraceOp::Assign    { var, has_env } => do_let(heap, frame, var, has_env),
            TraceOp::Call      { method, argc, has_recv }
                                       => return do_call(heap, frame, method, argc, has_recv),
            TraceOp::MakeProc { vargs, retval, body }
                                       => frame.local_stack.push(heap.alloc_proc(...)),
            TraceOp::Pop               => { frame.local_stack.pop(); }
        }
        StepResult::Continue
    } else if frame.tree_it < frame.expressions.len() {
        // 次の文へ。operations を作り直す。
        let next = frame.expressions[frame.tree_it].clone();
        frame.tree_it += 1;
        frame.operations.clear();
        frame.it = 0;
        trace(&next, &mut frame.operations);
        frame.operations.push(TraceOp::Pop); // 文の値は捨てる、ただし最後は別扱い
        StepResult::Continue
    } else {
        StepResult::EndOfFrame
    }
}
```

最終文の値を保つ規則（`local_stack.last()` を捨てない）は `expressions` の
最後の要素を判定して `Pop` を付けないことで実現する。

## 6. メソッド検索と呼び出し

`do_ref` の流れ（仕様 §3.3）:

1. 直接スロットを見る (`env.fields.get(var)`)。
2. 無ければ `env.__findSlot(env, var)` を呼ぶ。これはネイティブの
   `find_slot` （Binding 用は親チェーン、Class 用は instanceMethods 経由）。
3. それでも無ければ nil を返す。

`do_call` の流れ:

1. local_stack から argc 個 pop して args、has_recv なら recv も pop。
2. `find_method(heap, recv, method)` で `ProcKind` を取得。
3. `ProcKind` に応じて分岐:
   - `Native` → 即時実行し、戻り値を local_stack に push。`StepResult::Continue`。
   - `NativeSmash` → 現フレームの `operations`/`it`/`expressions` を書き換え
     て続行（csru の `DEFINE_SRU_PROC_SMASH` 相当）。
   - `Sru` → 新しい `StackFrame` を作って push し、評価ループに「新フレーム
     を切り替えて」と返す（`StepResult::Push(new_frame)`）。

## 7. 評価ループ

最外殻:

```rust
pub fn run(heap: &mut Heap, mut frame: StackFrame) -> ObjId {
    loop {
        match step(heap, &mut frame) {
            StepResult::Continue       => {}
            StepResult::Push(next)     => {
                // upper = 現フレームを保存、frame = next
                let upper = std::mem::replace(&mut frame, next);
                frame.upper = Some(Box::new(upper));
            }
            StepResult::EndOfFrame     => {
                // local_stack の末尾を上に積んで戻る
                let ret = frame.local_stack.pop().unwrap_or_else(|| heap.nil_id());
                match frame.upper.take() {
                    Some(up) => { frame = *up; frame.local_stack.push(ret); }
                    None     => return ret,
                }
            }
        }
    }
}
```

## 8. 継続

仕様 §spec_03 §5。

- `Proc` を作るとき `retval: Some(name)` があれば、`do_call` で SRU
  proc を起動する直前に「呼び出し元フレームのコピー」を作って継続オブ
  ジェクトに保持し、新フレームの binding に `name` 名で束縛する。
- 継続オブジェクト = `ObjData::Native(ContinationInvoke)` で、データに
  `frame: ObjId` （`ObjData::Frame` を保持する `BasicObject`）を持つ。
- 継続が呼ばれたら:
  1. 引数を 1 つ pop（無ければ nil）。
  2. `Interpreter::current_frame` を継続が握っていたフレームのコピーで
     上書き（csru `interpreter.cc:84-90` 相当）。
  3. 上書きしたフレームの local_stack に引数を push。
  4. 評価ループはそのまま継続。

実装メモ:
- 「現フレームを置き換える」のは Rust 的には `*frame = (*frame_ref).clone()`
  に相当する。`StackFrame` は `Clone`（`Expression` も `Clone`）で実装。
- フレームの `upper` 連鎖は **コピーしない**。コピーは先頭 1 個だけ。
  これにより継続の cost が O(1) になる（csru と同じ）。

## 9. ネイティブ手続きの呼び出し規約

```rust
pub type NativeFn      = fn(&mut Heap, &[ObjId]) -> ObjId;
pub type NativeSmashFn = fn(&mut Heap, &mut StackFrame, &[ObjId]);
```

- `Native` は引数を取って結果を返すだけ。受け手 (`recv`) は args[0] に
  入れて渡す。csru の `DEFINE_SRU_PROC(recv, args)` と同形。
- `NativeSmash` はフレームを直接書き換える。これが必要なのは `if`,
  `while`, `times` の本体実行のように **AST を後から実行させたい** ケース。

## 10. テスト

- 各 TraceOp の単体テスト（push/pop/lookup の挙動）。
- 仕様の例（`memo/spec_02_grammar.md §6` の脱糖サンプル）を AST 直 walk
  → 結果が想定通りか。
- csru の `samples/*.sru` を Rust 側で走らせ、stdout を csru の出力と
  diff する e2e テスト（段階 A 完了の合格判定）。
