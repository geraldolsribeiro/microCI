#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" cppcheck snapshot_create_script/cppcheck/input.yml snapshot_create_script/cppcheck/expected.sh
