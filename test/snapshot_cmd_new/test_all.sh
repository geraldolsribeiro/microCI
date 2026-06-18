#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered tests for `microCI --new <plugin>`.
# Each plugin has its own directory under test/snapshot_cmd_new/<plugin>/.
# The per-plugin test.sh wrapper invokes the shared runner.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pass=0
fail=0
skip=0

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  #beamer|docmd|doxygen|fetch|jfrog|minio|mermaid|pikchr|vhdl-format)

  case "$test_name" in
  npm | plugin_abc | plugin_xyz)
    echo -e "[cmd new all] ${YELLOW}SKIP${RESET}  $test_name"
    skip=$((skip + 1))
    continue
    ;;
  esac

  if "${dir}test.sh"; then
    echo -e "[cmd new gen] ${GREEN}PASS${RESET}  $test_name"
    echo -e "[cmd new ref] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
  else
    echo -e "[cmd new gen] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
  fi
done

echo "[cmd new] summary  pass=$pass fail=$fail skip=$skip"
exit "$fail"
