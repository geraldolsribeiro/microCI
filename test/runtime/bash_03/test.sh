#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"
test_name="$(basename "$script_dir")"

source ../helper.sh

# Objective: fail using apt without network
run_test_expect_failure
