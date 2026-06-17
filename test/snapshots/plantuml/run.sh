#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" plantuml snapshots/plantuml/input.yml snapshots/plantuml/expected.sh
