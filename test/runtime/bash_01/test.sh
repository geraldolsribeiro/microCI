#!/usr/bin/env bash
set -euo pipefail

# Per-test runtime wrapper.
# First verify the generated script executes successfully.
# Then run custom assertions (to be filled by the developer).
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
runtime_timeout="${RUNTIME_TIMEOUT:-120s}"
if ! "$script_dir/../runner_helper.sh" "bash_01" "$runtime_timeout"; then
  exit 1
fi

custom_output_check() {
  : # TODO: implement plugin-specific checks.
}

custom_output_check
