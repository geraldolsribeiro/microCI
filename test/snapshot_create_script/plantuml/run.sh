#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" plantuml snapshot_create_script/plantuml/input.yml snapshot_create_script/plantuml/expected.sh
