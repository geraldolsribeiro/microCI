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

xml_escape() {
  sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g' -e 's/"/\&quot;/g' -e "s/'/\&apos;/g"
}

add_result() {
  results+=("$1|$2|$3|$4")
}

write_junit() {
  local out="$1"
  local testsuite_name="$2"
  local total="${#results[@]}"
  local failures=0
  local skipped=0
  local testcase

  for testcase in "${results[@]}"; do
    IFS='|' read -r _name status _time _message <<<"$testcase"
    [[ "$status" == fail ]] && failures=$((failures + 1))
    [[ "$status" == skip ]] && skipped=$((skipped + 1))
  done

  {
    echo '<?xml version="1.0" encoding="UTF-8"?>'
    echo "<testsuite name=\"$testsuite_name\" tests=\"$total\" failures=\"$failures\" skipped=\"$skipped\">"
    for testcase in "${results[@]}"; do
      IFS='|' read -r name status time message <<<"$testcase"
      echo "  <testcase name=\"$(printf '%s' "$name" | xml_escape)\" time=\"$time\">"
      case "$status" in
        fail) echo "    <failure message=\"$(printf '%s' "$message" | xml_escape)\"/>" ;;
        skip) echo "    <skipped/>" ;;
      esac
      echo "  </testcase>"
    done
    echo "</testsuite>"
  } >"$out"
}
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
    add_result "$test_name" fail "$elapsed_s" "$(tail -n 20 "$output_file" | tr '\n' ' ' | sed 's/"/\\"/g')"
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
