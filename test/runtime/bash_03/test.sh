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

# Objective: fail using apt without network
expect_microci_failure() {
  microci_cmd='../../../bin/microCI | bash'
  if "$script_dir/../runner_helper.sh" "bash_03" "$microci_cmd"; then
    echo "[runtime] FAIL  bash_03: microCI succeeded but a failure was expected" >&2
    return 1
  fi
  return 0
}

if ! expect_microci_failure; then
  exit 1
fi

echo "[runtime] PASS  bash_03"
