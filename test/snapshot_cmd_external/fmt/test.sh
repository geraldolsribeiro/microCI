#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "fmt" "snapshot_cmd_external/fmt/expected.yml" "core.h"
