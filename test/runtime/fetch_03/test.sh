#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

source ../../test_helpers.sh

verify_runtime_output() {
  find external/ -type f >file.txt
  diff --color --unified file.txt file.ref
}

run_test_expect_success
