#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" plantuml snapshot_create_script/plantuml/input.yml snapshot_create_script/plantuml/expected.sh
