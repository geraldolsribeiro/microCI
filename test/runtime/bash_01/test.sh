#!/usr/bin/env bash
set -euo pipefail

# Per-test runtime wrapper.
# First verify the generated script executes successfully.
# Then run custom assertions (to be filled by the developer).
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

run_microci() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "bash_01" "$microci_cmd"
}

custom_output_check() {
  diff --color --unified file.txt file.ref
}

if ! run_microci; then
  echo "[runtime] FAIL  bash_01"
  exit 1
fi

if ! custom_output_check; then
  echo "[runtime] FAIL  bash_01"
  exit 1
fi

echo "[runtime] PASS  bash_01"
