#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered tests for `microCI --new <plugin>`.
# Each plugin has its own directory under test/snapshot_cmd_new/<plugin>/.
# The per-plugin test.sh wrapper invokes the shared runner.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pass=0
fail=0
skip=0
results=()
source "$script_dir/../test_helpers.sh"

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  #beamer|docmd|doxygen|fetch|jfrog|minio|mermaid|pikchr|vhdl-format)

  case "$test_name" in
  npm | plugin_abc | plugin_xyz)
    echo -e "[cmd new all] ${YELLOW}SKIP${RESET}  $test_name"
    skip=$((skip + 1))
    add_result "$test_name" skip 0 "fixture skipped by suite policy"
    continue
    ;;
  esac

  start_ns=$(date +%s%N)
  output_file=$(mktemp)
  if "${dir}test.sh" >"$output_file" 2>&1; then
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[cmd new gen] ${GREEN}PASS${RESET}  $test_name"
    echo -e "[cmd new ref] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
    add_result "$test_name" pass "$elapsed_s" ""
  else
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[cmd new gen] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
    add_result "$test_name" fail "$elapsed_s" "$(capture_tail_message "$output_file")"
  fi
  rm -f "$output_file"
done

xml_report="$script_dir/snapshot-cmd-new-junit.xml"
write_junit "$xml_report" "snapshot_cmd_new"

echo "[cmd new] summary  pass=$pass fail=$fail skip=$skip"
echo "[cmd new] junit    $xml_report"
exit "$fail"
