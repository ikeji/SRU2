# Working on this repo

## Always ulimit before running SRU programs

Both `csru/sru` and `rsru/target/release/rsru` can recurse into Rust /
C++ stacks via the `while` desugaring and via deep non-tail recursion,
and rsru's heap has no GC yet — it leaks. **Without limits, an unbounded
loop will consume RAM until the host swaps or OOM-kills processes.** I
already crashed this machine once that way. Don't do it again.

Default the testing shell to something like:

```
ulimit -s 8192    # 8 MB stack
ulimit -v 524288  # 512 MB virtual memory
ulimit -t 30      # 30 s CPU
```

Apply these before running any ad-hoc `*.sru` test, and especially
before any while-loop or recursive stress test. The repo's own
`tests/run.sh` and `cargo test` use bounded inputs so they're safe to
run without limits, but exploratory scripts in `/tmp/` are not.

## Repo layout

- `csru/` — original C++ reference implementation (the spec is whatever
  this does).
- `rsru/` — Rust port. `cargo build --release` builds the binary at
  `rsru/target/release/rsru`. `cargo test --release` runs integration
  tests.
- `tests/` — shared SRU integration tests. `tests/run.sh <interp>` runs
  them against either interpreter, with `--include-extensions` and
  `--include-rsru-only` flags for the optional subdirectories.
- `memo/spec_*.md` — extracted language spec from csru.
- `rsru/design_*.md` — porting design docs (Rust-side architecture).

## Running csru

Needs `libsru.so` on the loader path:

```
LD_LIBRARY_PATH=csru csru/sru tests/fizzbuzz.sru
```

## Useful gotchas (csru behaviour)

- `while (cond) ... end` — parens are required.
- `if cond then ... end` — `then` / `;` / `\n` separator required.
- Identifiers cannot start with `then` or `else` (the lexer eats the
  keyword greedily).
- `puts (x).toS()` parses as `puts(x).toS()`. Use an intermediate
  variable.
- `Array.new` returns the proc, `Array.new()` makes the array.
- `puts` is String-only; pass `x.toS()` for numerics, or use `p`.
- Boolean constants are `true` / `false`. csru's internal C++ uses the
  misspellings `tlue` / `farse` to dodge the C++ keywords, but those
  names are NOT bound at the SRU level.
- Many `Array` methods (`each`, `map`, `pop`, `shift`, `join`, ...)
  are `CHECK(false)` stubs in csru; rsru ships SRU implementations in
  `rsru/src/prelude.sru`.

## Useful gotchas (rsru-specific)

- No GC. Each iteration of a long-running loop allocates BasicObjects
  (procs, bindings) that aren't collected. Don't run millions of
  iterations without `ulimit -v`.
- TCO fires only on direct tail positions. `n + f(n-1)` is not a tail
  call and will consume memory linearly with recursion depth.
- The `__parser` object is exposed exactly as csru does; SRU code can
  replace `__parser.const_literal` etc. to extend the grammar at
  runtime. See `tests/parser_extension/hex_literal.sru`.
