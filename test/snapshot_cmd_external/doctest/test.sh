#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "doctest" "snapshot_cmd_external/doctest/expected.yml"
