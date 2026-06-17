#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" "clang-format" "snapshot_cmd_new/clang-format/expected.yml"
