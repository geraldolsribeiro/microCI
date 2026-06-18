#!/usr/bin/env bash
set -euo pipefail

# Regenerate snapshot baselines in canonical shell format.
# This keeps expected.sh files in sync with the generator and shfmt.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

for dir in */; do
  echo "Scanning $dir..."
  [[ -f "$dir/input.yml" ]] || continue
  # [[ -f "$dir/expected.sh" ]] || continue

  test_name=${dir%/}
  case "$test_name" in
  plugin_abc | plugin_xyz)
    echo "[create script] SKIP  $test_name"
    continue
    ;;
  esac

  pushd "$dir" >/dev/null
  ../../../bin/microCI -i "input.yml" | shfmt -i 2 -ci -sr >"expected.sh"
  popd >/dev/null

  echo "[create script] UPDATED  $test_name"
done
