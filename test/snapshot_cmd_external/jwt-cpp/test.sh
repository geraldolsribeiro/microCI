#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "jwt-cpp" "snapshot_cmd_external/jwt-cpp/expected.yml" "jwt.h"
