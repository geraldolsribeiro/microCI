#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "clang-format" "snapshot_cmd_new/clang-format/expected.yml"
