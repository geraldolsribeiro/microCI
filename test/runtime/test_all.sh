#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

# Auto-discovered runtime tests under test/runtime/<plugin>_<NN>/.
#
# Keep this file focused on suite orchestration so future improvements can be
# made in the per-test scripts without rewriting discovery/summary logic.
# Each per-test test.sh wrapper invokes the shared runner.
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pass=0
fail=0
skip=0
results=()
source "$script_dir/../test_helpers.sh"

for dir in "$script_dir"/*/; do
  [[ -f "${dir}test.sh" ]] || continue
  test_name="$(basename "$dir")"

  case "$test_name" in
  clang-tidy_* | cppcheck_* | fetch_* | flawfinder_* | git_deploy_* | git_publish_* | mkdocs_material_* | plantuml_* | skip_* | template_* | beamer_* | docmd_* | doxygen_* | jfrog_* | minio_* | npm_* | mermaid_* | pikchr_* | vhdl-format_*)
    # Placeholder skip list for fixtures that still need concrete assertions.
    # Future improvement: move this metadata next to each fixture.
    echo -e "[runtime] ${YELLOW}SKIP${RESET}  $test_name"
    skip=$((skip + 1))
    add_result "$test_name" skip 0 "fixture skipped by suite policy"
    continue
    ;;
  esac

  start_ns=$(date +%s%N)
  runtime_timeout="${RUNTIME_TIMEOUT:-120s}"
  output_file=$(mktemp)
  if timeout "$runtime_timeout" "${dir}test.sh" >"$output_file" 2>&1; then
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[runtime] ${GREEN}PASS${RESET}  $test_name"
    pass=$((pass + 1))
    add_result "$test_name" pass "$elapsed_s" ""
  else
    elapsed_ns=$(( $(date +%s%N) - start_ns ))
    elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.3f", ns / 1000000000 }')
    echo -e "[runtime] ${RED}FAIL${RESET}  $test_name"
    fail=$((fail + 1))
    add_result "$test_name" fail "$elapsed_s" "$(capture_tail_message "$output_file")"
  fi
  rm -f "$output_file"
done

xml_report="$script_dir/runtime-junit.xml"
write_junit "$xml_report" "runtime"

echo "[runtime] summary  pass=$pass fail=$fail skip=$skip"
echo "[runtime] junit    $xml_report"
exit "$fail"
