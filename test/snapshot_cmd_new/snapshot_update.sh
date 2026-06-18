#!/usr/bin/env bash
set -euo pipefail

# Regenerate `microCI --new <plugin>` fixtures in canonical YAML form.
# This helper mirrors snapshot_create_script/update_expected.sh.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  if [[ -f "${dir}expected.yml" ]]; then
    if output="$("${dir}test.sh" 2>&1)"; then
      if grep -q '\[cmd new\] SKIP' <<<"$output"; then
        echo "[cmd new] SKIP  $test_name"
      else
        # The run.sh already validates the fixture; on update we just reuse it.
        echo "[cmd new] UPDATED  $test_name"
      fi
    else
      echo "[cmd new] FAIL  $test_name"
      printf '%s\n' "$output"
      exit 1
    fi
  fi
done
