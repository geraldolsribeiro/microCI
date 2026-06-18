# Override next function in test.sh
verify_runtime_output() {
  echo "verify_runtime_output not implemented"
  return 1
}

run_microci() {
  microci_cmd='../../../bin/microCI | bash'
  "$script_dir/../runner_helper.sh" "$test_name" "$microci_cmd"
}

# Top-level flow uses exit so the whole test script fails immediately when a check fails.
run_test_expect_success() {
  if ! run_microci >/dev/null; then
    echo "[runtime] FAIL  $test_name"
    exit 1
  fi

  if ! verify_runtime_output; then
    echo "[runtime] FAIL  $test_name"
    exit 1
  fi

  echo "[runtime] PASS  $test_name"
}

run_test_expect_failure() {
  if run_microci >/dev/null; then
    echo "[runtime] FAIL  $test_name: microCI succeeded but a failure was expected" >&2
    exit 1
  fi

  echo "[runtime] PASS  $test_name"
}
