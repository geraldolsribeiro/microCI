#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered runtime tests under test/runtime/<plugin>_<NN>/.
#
# Keep this file focused on suite orchestration so future improvements can be
# made in the per-test scripts without rewriting discovery/summary logic.
# Each per-test test.sh wrapper invokes the shared runner.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pass=0
fail=0
skip=0

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  case "$test_name" in
    beamer_*|docmd_*|doxygen_*|jfrog_*|minio_*|npm_*|mermaid_*|pikchr_*|vhdl-format_*)
      # Placeholder skip list for fixtures that still need concrete assertions.
      # Future improvement: move this metadata next to each fixture.
      echo -e "[runtime] ${YELLOW}SKIP${RESET}  $test_name"
      skip=$((skip + 1))
      continue
      ;;
  esac

  runtime_timeout="${RUNTIME_TIMEOUT:-120s}"
  if timeout "$runtime_timeout" "${dir}test.sh" >/dev/null; then
    echo -e "[runtime] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
  else
    echo -e "[runtime] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
  fi
done

echo "[runtime] summary  pass=$pass fail=$fail skip=$skip"
exit "$fail"
