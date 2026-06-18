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
#
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"
self_name="$(basename "$script_dir")"

expect_microci_success() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "bash_01" "$microci_cmd"
}

verify_runtime_output() {
  # Return a status code instead of exiting so the caller can keep the control
  # flow explicit. This makes it easier for future tests to add multiple checks
  # and decide how to report each failure.
  diff --color --unified file.txt file.ref
}

if ! expect_microci_success; then
  echo "[runtime] FAIL  bash_01"
  exit 1
fi

# Top-level flow uses exit so the whole test script fails immediately when a
# check fails.
if ! verify_runtime_output; then
  echo "[runtime] FAIL  bash_01"
  exit 1
fi

echo "[runtime] PASS  bash_01"
