#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" npm snapshot_create_script/npm/input.yml snapshot_create_script/npm/expected.sh
