#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Run every snapshot test directory found under this folder.
# Each directory must provide a test.sh wrapper.
# We compare canonical shell output (via shfmt) so whitespace-only changes
# do not fail snapshots.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

pass=0
fail=0
skip=0
results=()
source "$script_dir/../test_helpers.sh"
for dir in */; do
  # Ignore non-test directories.
  [[ -f "$dir/test.sh" ]] || continue

  test_name=${dir%/}
  # Skip fixtures that are not valid or not ready yet.
  # These are retained as placeholders to document planned coverage.
  # npm | docmd | doxygen | jfrog | minio)
  case "$test_name" in
  npm | plugin_abc | plugin_xyz)
    echo -e "[create script] ${YELLOW}SKIP${RESET}  $test_name"
    skip=$((skip + 1))
    add_result "$test_name" skip 0 "fixture skipped by suite policy"
    continue
    ;;
  esac

  start_ns=$(date +%s%N)
  output_file=$(mktemp)
  pushd "$script_dir/$dir" >/dev/null
  if "./test.sh" >"$output_file" 2>&1; then
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[create script] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
    add_result "$test_name" pass "$elapsed_s" ""
  else
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[create script] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
    add_result "$test_name" fail "$elapsed_s" "$(capture_tail_message "$output_file")"
  fi
  popd >/dev/null
  rm -f "$output_file"
done

# Summary is intentionally compact for CI logs.
xml_report="$script_dir/snapshot-create-script-junit.xml"
write_junit "$xml_report" "snapshot_create_script"

echo "[create script] summary  pass=$pass fail=$fail skip=$skip"
echo "[create script] junit    $xml_report"
exit "$fail"
