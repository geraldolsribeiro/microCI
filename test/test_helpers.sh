#!/usr/bin/env bash
set -euo pipefail

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

capture_tail_message() {
  local output_file="$1"
  tail -n 20 "$output_file" | tr '\n' ' ' | sed 's/"/\\"/g'
}

# Per-test wrappers may override these after sourcing this helper.
setup_test() {
  :
}

teardown_test() {
  :
}

run_microci() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "$test_name" "$microci_cmd"
}

# Runtime test helpers.
run_test_expect_success() {
  setup_test

  if ! run_microci >/dev/null; then
    echo "[runtime] FAIL  $test_name"
    teardown_test
    exit 1
  fi

  if ! verify_runtime_output; then
    echo "[runtime] FAIL  $test_name"
    teardown_test
    exit 1
  fi

  echo "[runtime] PASS  $test_name"
  teardown_test
}

run_test_expect_failure() {
  setup_test

  if run_microci >/dev/null; then
    echo "[runtime] FAIL  $test_name: microCI succeeded but a failure was expected" >&2
    teardown_test
    exit 1
  fi

  echo "[runtime] PASS  $test_name"
  teardown_test
}
