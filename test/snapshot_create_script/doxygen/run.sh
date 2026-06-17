#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" doxygen snapshot_create_script/doxygen/input.yml snapshot_create_script/doxygen/expected.sh
