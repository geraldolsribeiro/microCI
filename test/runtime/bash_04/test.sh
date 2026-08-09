#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

source ../../test_helpers.sh

verify_runtime_output() {
  # when ran in MacOS uses a arm64 debian image
  sed -i.bak "s/arm64/amd64/g" apt.txt

  diff --color --unified apt.txt apt.ref
}

run_test_expect_success
