#!/usr/bin/env bash
set -euo pipefail

# Shared runner for `microCI --external <name>` tests.
# It is invoked by per-name wrappers discovered under test/snapshot_cmd_external/.
#
# Command chain:
#   make -C test snapshot_cmd_external_test
#     -> ./snapshot_cmd_external/test_all.sh
#       -> ./snapshot_cmd_external/<name>/test.sh
#         -> ./snapshot_cmd_external/runner_helper.sh <name> <expected.yml>
#
# This helper performs both sub-tests for a name:
# - compare the generated `microCI --external` YAML against the name fixture
# - compare the fixture against the canonical `../external/<name>.yml`
if [[ $# -ne 3 ]]; then
  echo "Usage: $0 <name> <expected.yml> <filename>" >&2
  exit 2
fi

name="$1"
expected_rel="$2"
filename="$3"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
cd "$repo_root/test"

# Sub-test 1: generated output vs fixture.
tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

(cd "$tmpdir" && "$repo_root/bin/microCI" --external "$name" >/dev/null)

# Execute the step
(cd "$tmpdir" && $repo_root/bin/microCI | bash)

find "$tmpdir" -name "$filename"

diff --color --unified "$expected_rel" "$tmpdir/.microCI.yml" >/dev/null

# Sub-test 2: fixture vs canonical template.
canonical_expected="$repo_root/external/${name}.yml"
if [[ -f "$canonical_expected" && -f "$expected_rel" ]]; then
  diff --color --unified "$canonical_expected" "$expected_rel" >/dev/null
fi
