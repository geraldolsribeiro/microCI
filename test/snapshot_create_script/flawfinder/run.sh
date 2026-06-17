#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" flawfinder snapshot_create_script/flawfinder/input.yml snapshot_create_script/flawfinder/expected.sh
