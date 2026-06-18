#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

source ../helper.sh

verify_runtime_output() {
  diff --color --unified apt.txt apt.ref
}

run_test_expect_success
