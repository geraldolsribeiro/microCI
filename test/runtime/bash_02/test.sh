#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

source ../helper.sh

# Objective: fail executing admin commands as user
run_test_expect_failure
