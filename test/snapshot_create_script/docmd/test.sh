#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" docmd snapshot_create_script/docmd/input.yml snapshot_create_script/docmd/expected.sh
