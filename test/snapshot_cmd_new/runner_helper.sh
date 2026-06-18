#!/usr/bin/env bash
set -euo pipefail

# Shared runner for `microCI --new <plugin>` tests.
# It is invoked by per-plugin wrappers discovered under test/snapshot_cmd_new/.
#
# Command chain:
#   make -C test snapshot_cmd_new_test
#     -> ./snapshot_cmd_new/test_all.sh
#       -> ./snapshot_cmd_new/<plugin>/test.sh
#         -> ./snapshot_cmd_new/runner_helper.sh <plugin> <expected.yml>
#
# This helper performs both sub-tests for a plugin:
# - compare the generated `microCI --new` YAML against the plugin fixture
# - compare the fixture against the canonical `../new/<plugin>.yml`
if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <plugin> <expected.yml>" >&2
  exit 2
fi

plugin="$1"
expected_rel="$2"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
cd "$repo_root/test"

# Sub-test 1: generated output vs fixture.
tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT
(cd "$tmpdir" && "$repo_root/bin/microCI" --new "$plugin" >/dev/null)
diff --color --unified "$expected_rel" "$tmpdir/.microCI.yml" >/dev/null

# Sub-test 2: fixture vs canonical template.
canonical_expected="$repo_root/new/${plugin}.yml"
if [[ -f "$canonical_expected" && -f "$expected_rel" ]]; then
  diff --color --unified "$canonical_expected" "$expected_rel" >/dev/null
fi
