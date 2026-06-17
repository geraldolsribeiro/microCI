#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" "cppcheck" "snapshot_cmd_new/cppcheck/expected.yml"
