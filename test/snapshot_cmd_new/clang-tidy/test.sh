#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "clang-tidy" "snapshot_cmd_new/clang-tidy/expected.yml"
