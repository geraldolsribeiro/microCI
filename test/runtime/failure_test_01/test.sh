#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

# This fixture reproduces the known failure regression scenario.
# It should fail on the bad middle step and never reach the last step.
workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

cp "$repo_root/test/failure_test/fail_gcc.yml" "$workdir/.microCI.yml"

if (cd "$workdir" && "$repo_root/bin/microCI" | bash) >"$workdir/output.txt" 2>&1; then
  echo "[runtime] FAIL  failure_test_01: pipeline succeeded but step 2 should fail"
  exit 1
fi

# Positive checks: the first two steps should appear.
grep -q "Good step 01" "$workdir/output.txt"
grep -q "Bad step 01 - Failure should stop pipeline" "$workdir/output.txt"

# Negative check: the last step must never run after the failure.
if grep -q "Good step 02 - This step never execute" "$workdir/output.txt"; then
  echo "[runtime] FAIL  failure_test_01: pipeline continued after failure"
  exit 1
fi

echo "[runtime] PASS  failure_test_01"
