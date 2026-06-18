#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" skip snapshot_create_script/skip/input.yml snapshot_create_script/skip/expected.sh
