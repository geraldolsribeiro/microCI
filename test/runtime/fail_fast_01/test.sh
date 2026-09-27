#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

# Isolate the run so this test does not touch the repository state.
workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

# Use a tiny pipeline fixture with 5 steps.
cp "$script_dir/.microCI.yml" "$workdir/.microCI.yml"

# microCI should fail because step 3 runs `false`.
if (cd "$workdir" && "$repo_root/bin/microCI" | bash) >"$workdir/output.txt" 2>&1; then
  echo "[runtime] FAIL  fail_fast_01: pipeline succeeded but step 3 should fail"
  exit 1
fi

# Positive check: the first three steps must appear in the output.
grep -q "STEP 1" "$workdir/output.txt"
grep -q "STEP 2" "$workdir/output.txt"
grep -q "STEP 3" "$workdir/output.txt"

# Negative check: once step 3 fails, step 4 and 5 must not run.
# This is the key assertion for the fail-fast behavior.
if grep -q "STEP 4" "$workdir/output.txt" || grep -q "STEP 5" "$workdir/output.txt"; then
  echo "[runtime] FAIL  fail_fast_01: pipeline continued after step 3 failure"
  exit 1
fi

echo "[runtime] PASS  fail_fast_01"
