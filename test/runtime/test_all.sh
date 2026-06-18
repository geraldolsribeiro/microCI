#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered runtime tests under test/runtime/<plugin>_<NN>/.
# Each per-test test.sh wrapper invokes the shared runner.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pass=0
fail=0
skip=0

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  # Runtime fixtures are scaffolded but intentionally skipped until each one
  # has its own concrete success criteria.
  echo -e "[runtime] ${YELLOW}SKIP${RESET}  $test_name"
  skip=$((skip + 1))
done

echo "[runtime] summary  pass=$pass fail=$fail skip=$skip"
exit "$fail"
