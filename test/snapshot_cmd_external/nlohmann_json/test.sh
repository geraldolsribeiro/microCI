#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "nlohmann_json" "snapshot_cmd_external/nlohmann_json/expected.yml" "json.hpp"
