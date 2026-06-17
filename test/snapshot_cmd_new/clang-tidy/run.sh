#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" "clang-tidy" "snapshot_cmd_new/clang-tidy/expected.yml"
