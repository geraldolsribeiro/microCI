#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Run every snapshot test directory found under this folder.
# Each directory must provide a run.sh wrapper.
# We compare canonical shell output (via shfmt) so whitespace-only changes
# do not fail snapshots.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

pass=0
fail=0
skip=0
for dir in */; do
  # Ignore non-test directories.
  [[ -f "$dir/run.sh" ]] || continue

  test_name=${dir%/}
  # Skip fixtures that are not valid or not ready yet.
  # These are retained as placeholders to document planned coverage.
  case "$test_name" in
    npm|beamer|docmd|doxygen|jfrog|minio)
      echo -e "[create script] ${YELLOW}SKIP${RESET}  $test_name"
      skip=$((skip + 1))
      continue
      ;;
  esac

  # run.sh handles execution, normalization, and diff reporting.
  if "$script_dir/$dir/run.sh" >/dev/null; then
    echo -e "[create script] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
  else
    echo -e "[create script] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
  fi
done

# Summary is intentionally compact for CI logs.
echo "[create script] summary  pass=$pass fail=$fail skip=$skip"
exit "$fail"
