#!/usr/bin/env bash
set -euo pipefail

# Per-test runtime wrapper.
#
# This test is intentionally inverted: it is successful only when microCI
# fails on invalid YAML. Future failure-oriented runtime tests can copy this
# pattern and keep the expectation obvious next to the fixture.
#
# Flow:
# - run microCI using the shared helper
# - assert that the helper fails
# - run any additional checks if needed
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

expect_microci_success() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "bash_04" "$microci_cmd"
}

verify_runtime_output() {
  # Return a status code instead of exiting so the caller can keep the control
  # flow explicit. This makes it easier for future tests to add multiple checks
  # and decide how to report each failure.
  diff --color --unified apt.txt apt.ref
}

if ! expect_microci_success; then
  echo "[runtime] FAIL  bash_04"
  exit 1
fi

# Top-level flow uses exit so the whole test script fails immediately when a
# check fails.
if ! verify_runtime_output; then
  echo "[runtime] FAIL  bash_04"
  exit 1
fi

echo "[runtime] PASS  bash_04"
