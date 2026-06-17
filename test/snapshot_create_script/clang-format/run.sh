#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" clang-format snapshot_create_script/clang-format/input.yml snapshot_create_script/clang-format/expected.sh
