#!/usr/bin/env bash
set -euo pipefail

# Shared runner for runtime tests.
# It is called by per-test wrappers under test/runtime/<plugin>_<NN>/test.sh.
#
# Command chain:
#   make -C test runtime_test
#     -> ./runtime/test_all.sh
#       -> ./runtime/<plugin>_<NN>/test.sh
#         -> ./runtime/runner_helper.sh <name>
#
# This helper only executes `microCI | bash` and reports its result.
# Per-test scripts may add custom output verification after this call.
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <name>" >&2
  exit 2
fi

name="$1"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
cd "$repo_root/test"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

if timeout 120s "$repo_root/bin/microCI" | bash >"$tmpdir/out.log" 2>&1; then
  echo "[runtime] PASS  $name"
  return_code=0
else
  echo "[runtime] FAIL  $name"
  cat "$tmpdir/out.log"
  return_code=1
fi

exit "$return_code"
