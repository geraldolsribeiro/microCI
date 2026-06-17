#!/usr/bin/env bash
set -euo pipefail

# Shared runner for shell-script snapshot tests.
#
# Do not call this file directly in normal use. It is intended to be invoked by
# the per-snapshot run.sh wrappers (which are in turn executed by
# snapshot_test.sh via the Makefile).
#
# Full command chain during CI/local runs:
#   make -C test snapshot_test
#     -> ./snapshots/snapshot_test.sh
#       -> ./<plugin>/run.sh
#         -> ./run_snapshot.sh <name> <input.yml> <expected.sh>
#
# It generates a script from a fixture, normalizes unstable parts, formats the
# output with shfmt, and compares it with the stored golden file.
if [[ $# -ne 3 ]]; then
  echo "Usage: $0 <name> <input.yml> <expected.sh>" >&2
  exit 2
fi

name="$1"
input_rel="$2"
expected_rel="$3"

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$script_dir/../.."
cd "$repo_root/test"

# Temporary workspace used to keep the comparison isolated and deterministic.
workdir=$(mktemp -d)
trap 'rm -rf "$workdir"' EXIT

cp "$input_rel" "$workdir/input.yml"
../bin/microCI -i "$workdir/input.yml" >"$workdir/out.sh"

normalize() {
  # Keep snapshots stable by removing runtime-only paths.
  sed -E 's#/tmp/tmp\.[^/]+#<TMPDIR>#g; s#<TMPDIR>/input\.yml#<INPUT>#g; s#snapshots/[^" ]+/input\.yml#<INPUT>#g; s#[A-Za-z0-9_-]+//input\.yml#<INPUT>#g'
}

# Compare shell scripts structurally, not by whitespace or line wrapping.
normalize <"$expected_rel" | shfmt -i 2 -ci -sr >"$workdir/expected.norm"
normalize <"$workdir/out.sh" | shfmt -i 2 -ci -sr >"$workdir/out.norm"

if diff --color --unified "$workdir/expected.norm" "$workdir/out.norm"; then
  echo "[snapshot] PASS  $name"
  status=0
else
  echo "[snapshot] FAIL  $name"
  status=1
fi

if [[ ${SNAPSHOT_SUMMARY:-0} == 1 ]]; then
  echo "[snapshot] done   $name"
fi

exit "$status"
