#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" mkdocs_material snapshots/mkdocs_material/input.yml snapshots/mkdocs_material/expected.sh
