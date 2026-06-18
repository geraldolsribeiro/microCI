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
if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo "Usage: $0 <name> [timeout]" >&2
  exit 2
fi

name="$1"
timeout="${2:-120s}"
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."

tmpdir="$(mktemp -d)"
cleanup() {
  rm -rf "$tmpdir"
}
trap cleanup EXIT

if timeout "$timeout" "$repo_root/bin/microCI" | bash >"$tmpdir/out.log" 2>&1; then
  echo "[runtime] PASS  $name"
  return_code=0
else
  timeout_status=$?
  if [[ $timeout_status -eq 124 ]]; then
    # Best-effort cleanup for orphaned containers created by timed-out runs.
    docker ps -q --filter "name=^/microci_" | xargs -r docker kill >/dev/null 2>&1 || true
    docker ps -aq --filter "name=^/microci_" | xargs -r docker rm -f >/dev/null 2>&1 || true
    echo "[runtime] FAIL  $name (timeout)"
  else
    echo "[runtime] FAIL  $name"
  fi
  cat "$tmpdir/out.log"
  return_code=1
fi

exit "$return_code"
