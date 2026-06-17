#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" clang-tidy snapshot_create_script/clang-tidy/input.yml snapshot_create_script/clang-tidy/expected.sh
