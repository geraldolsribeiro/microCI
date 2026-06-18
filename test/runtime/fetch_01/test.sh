#!/usr/bin/env bash
set -euo pipefail

# Per-test runtime wrapper.
#
# Keep this file small and easy to customize per plugin. Future runtime tests
# can replace the placeholder assertions with plugin-specific checks without
# changing the shared runner.
#
# Flow:
# - run microCI | bash using the local wrapper
# - verify the runtime output in a dedicated helper function
# - return from helpers, exit from top-level flow
# - this is the default success-oriented skeleton for fetch_01
#
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

run_microci() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "$test_name" "$microci_cmd"
}

verify_runtime_output() {
  # Replace this placeholder with plugin-specific assertions.
  # Return 0 when the runtime output is acceptable.
  echo "[runtime] FAIL  $test_name: runtime verification not implemented" >&2
  return 1
}

if ! run_microci; then
  echo "[runtime] FAIL  $test_name"
  exit 1
fi

if ! verify_runtime_output; then
  echo "[runtime] FAIL  $test_name"
  exit 1
fi

echo "[runtime] PASS  $test_name"
