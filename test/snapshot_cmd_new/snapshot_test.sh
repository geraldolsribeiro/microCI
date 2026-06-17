#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered tests for `microCI --new <plugin>`.
# Each plugin has its own directory under test/snapshot_cmd_new/<plugin>/.
# The shared runner compares generated YAML with the canonical expected.yml.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
pass=0
fail=0
skip=0

for dir in "$script_dir"/*/; do
  [[ -f "${dir}run.sh" ]] || continue
  test_name="$(basename "$dir")"

  if output="$("${dir}run.sh" 2>&1)"; then
    if grep -q '\[cmd new gen\] SKIP' <<<"$output"; then
      echo -e "[cmd new gen] ${YELLOW}SKIP${RESET}  $test_name"
      skip=$((skip + 1))
    else
      echo -e "[cmd new gen] ${GREEN}PASS${RESET}  $test_name"
      pass=$((pass + 1))
    fi
  else
    echo -e "[cmd new gen] ${RED}FAIL${RESET}  $test_name"
    printf '%s\n' "$output"
    fail=$((fail + 1))
  fi

  canonical_expected="$repo_root/new/${test_name}.yml"
  fixture_expected="${dir}expected.yml"
  if [[ -f "$canonical_expected" && -f "$fixture_expected" ]]; then
    if diff --color --unified "$canonical_expected" "$fixture_expected" >/dev/null; then
      echo -e "[cmd new ref] ${GREEN}PASS${RESET}  $test_name"
    else
      echo -e "[cmd new ref] ${RED}FAIL${RESET}  $test_name"
      diff --color --unified "$canonical_expected" "$fixture_expected" || true
      fail=$((fail + 1))
    fi
  fi
done

echo "[cmd new gen] summary  pass=$pass fail=$fail skip=$skip"
exit "$fail"
