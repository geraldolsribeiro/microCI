#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

cp "$script_dir/.microCI.yml" "$workdir/.microCI.yml"

# Run the generated pipeline. It should fail on step 3.
# The bug is that steps 4 and 5 may still run after the failure.
if (cd "$workdir" && "$repo_root/bin/microCI" | bash) >"$workdir/output.txt" 2>&1; then
  echo "[runtime] FAIL  fail_fast_02: pipeline succeeded but step 3 should fail"
  exit 1
fi

# The first three steps should have executed and appended to the log.
grep -qx "1" "$workdir/steps.log"
grep -qx "2" "$workdir/steps.log"
grep -qx "3" "$workdir/steps.log"

# Negative assertion: after step 3 fails, step 4 and 5 must not run.
# If the bug is present, their markers will appear in steps.log.
if grep -qx "4" "$workdir/steps.log" || grep -qx "5" "$workdir/steps.log"; then
  echo "[runtime] FAIL  fail_fast_02: pipeline continued after step 3 failure"
  echo "--- steps.log ---"
  cat "$workdir/steps.log"
  exit 1
fi

echo "[runtime] PASS  fail_fast_02"
