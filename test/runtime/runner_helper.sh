#!/usr/bin/env bash
set -euo pipefail

# Shared runner for runtime tests.
#
# Keep this file generic: it should only execute the provided command and
# manage common concerns (output capture, timeout cleanup, exit status).
# Per-test wrappers own the actual microCI command line and assertions.
#
# It is called by per-test wrappers under test/runtime/<plugin>_<NN>/test.sh.
#
# Command chain:
#   make -C test runtime_test
#     -> ./runtime/test_all.sh
#       -> ./runtime/<plugin>_<NN>/test.sh
#         -> ./runtime/runner_helper.sh <name>
#
# This helper only executes the provided shell command and reports its result.
# Future improvements can extend this runner with richer diagnostics, artifact
# collection, or per-test log capture without duplicating logic in test.sh.
if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <name> <command>" >&2
  exit 2
fi

name="$1"
command="$2"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."

tmpdir="$(mktemp -d)"
cleanup() {
  rm -rf "$tmpdir"
}
trap cleanup EXIT

if bash -lc "$command" >"$tmpdir/out.log" 2>&1; then
  cat "$tmpdir/out.log"
  exit 0
fi

timeout_status=$?
if [[ $timeout_status -eq 124 ]]; then
  # Best-effort cleanup for orphaned containers created by timed-out runs.
  docker ps -q --filter "name=^/microci_" | xargs -r docker kill >/dev/null 2>&1 || true
  docker ps -aq --filter "name=^/microci_" | xargs -r docker rm -f >/dev/null 2>&1 || true
fi
cat "$tmpdir/out.log"
exit 1
