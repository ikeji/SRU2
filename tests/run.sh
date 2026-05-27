#!/bin/bash
# Run every *.sru in this directory against the given interpreter and diff
# stdout with the matching *.expected file.
#
# Usage:
#   tests/run.sh                  # default: csru/sru with LD_LIBRARY_PATH=csru
#   tests/run.sh path/to/rsru     # use a different interpreter
#
# Returns nonzero if any test fails. The parser_extension/ subdir is skipped
# unless --include-extensions is passed (csru cannot parse those tests).

set -e

here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/.." && pwd)

interp="${1:-}"
include_ext=0
for arg in "$@"; do
  case "$arg" in
    --include-extensions) include_ext=1 ;;
  esac
done

if [ -z "$interp" ] || [ "$interp" = "--include-extensions" ]; then
  interp="$root/csru/sru"
  export LD_LIBRARY_PATH="$root/csru:${LD_LIBRARY_PATH:-}"
fi

fail=0
pass=0
for src in "$here"/*.sru; do
  name=$(basename "$src" .sru)
  expected="$here/$name.expected"
  if [ ! -f "$expected" ]; then
    echo "SKIP $name (no .expected)"
    continue
  fi
  got=$("$interp" "$src" 2>/dev/null || true)
  if [ "$got" = "$(cat "$expected")" ]; then
    echo "PASS $name"
    pass=$((pass + 1))
  else
    echo "FAIL $name"
    diff <(echo "$got") "$expected" | sed 's/^/  /'
    fail=$((fail + 1))
  fi
done

if [ "$include_ext" = "1" ]; then
  for src in "$here"/parser_extension/*.sru; do
    name=$(basename "$src" .sru)
    expected="$here/parser_extension/$name.expected"
    [ -f "$expected" ] || continue
    got=$("$interp" "$src" 2>/dev/null || true)
    if [ "$got" = "$(cat "$expected")" ]; then
      echo "PASS parser_extension/$name"
      pass=$((pass + 1))
    else
      echo "FAIL parser_extension/$name"
      diff <(echo "$got") "$expected" | sed 's/^/  /'
      fail=$((fail + 1))
    fi
  done
fi

echo
echo "Passed: $pass, Failed: $fail"
[ "$fail" -eq 0 ]
