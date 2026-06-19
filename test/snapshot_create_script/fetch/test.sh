#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" fetch snapshot_create_script/fetch/input.yml snapshot_create_script/fetch/expected.sh
