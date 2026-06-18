#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" flawfinder snapshot_create_script/flawfinder/input.yml snapshot_create_script/flawfinder/expected.sh
