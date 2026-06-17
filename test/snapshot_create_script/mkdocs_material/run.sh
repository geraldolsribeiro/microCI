#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" mkdocs_material snapshot_create_script/mkdocs_material/input.yml snapshot_create_script/mkdocs_material/expected.sh
