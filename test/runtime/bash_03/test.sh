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
test_name="$(basename "$script_dir")"

# Objective: fail using apt without network
expect_microci_failure() {
  microci_cmd='../../../bin/microCI | bash'
  if "$script_dir/../runner_helper.sh" "test_name" "$microci_cmd"; then
    echo "[runtime] FAIL  $test_name: microCI succeeded but a failure was expected" >&2
    return 1
  fi
  return 0
}

if ! expect_microci_failure; then
  exit 1
fi

echo "[runtime] PASS  test_name"
