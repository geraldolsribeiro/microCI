#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "spdlog" "snapshot_cmd_external/spdlog/expected.yml" "spdlog.h"
