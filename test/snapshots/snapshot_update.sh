#!/usr/bin/env bash
set -euo pipefail

# Regenerate snapshot baselines in canonical shell format.
# This keeps expected.sh files in sync with the generator and shfmt.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

for dir in */; do
  [[ -f "$dir/input.yml" ]] || continue
  [[ -f "$dir/expected.sh" ]] || continue

  test_name=${dir%/}
  case "$test_name" in
    npm|beamer|docmd|doxygen|jfrog|minio)
      echo "[snapshot] SKIP  $test_name"
      continue
      ;;
  esac

  ../../bin/microCI -i "$dir/input.yml" | shfmt -i 2 -ci -sr > "$dir/expected.sh"
  echo "[snapshot] UPDATED  $test_name"
done
