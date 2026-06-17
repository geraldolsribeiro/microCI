#!/usr/bin/env bash
set -euo pipefail

# Shared runner for `microCI --new <plugin>` tests.
# It is invoked by per-plugin wrappers discovered under test/snapshot_cmd_new/.
#
# Command chain:
#   make -C test snapshot_cmd_new_test
#     -> ./snapshot_cmd_new_test.sh
#       -> ./snapshot_cmd_new/<plugin>/run.sh
#         -> ./snapshot_cmd_new/run_new.sh <plugin> <expected.yml>
if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <plugin> <expected.yml>" >&2
  exit 2
fi

plugin="$1"
expected_rel="$2"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
cd "$repo_root/test"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

(cd "$tmpdir" && "$repo_root/bin/microCI" --new "$plugin" >/dev/null)

# Compare canonical YAML output so formatting-only changes do not fail tests.
diff --color --unified "$expected_rel" "$tmpdir/.microCI.yml"
