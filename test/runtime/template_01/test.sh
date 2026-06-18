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
run_microci() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "template_01" "$microci_cmd" ;
}

if ! run_microci; then
  exit 1
fi

verify_runtime_output() {
  echo "[runtime] FAIL  ${test_name}: verify_runtime_output not implemented" >&2
  return 1
}

if ! verify_runtime_output; then
  echo "[runtime] FAIL  ${test_name}"
  exit 1
fi

echo "[runtime] PASS  ${test_name}"
