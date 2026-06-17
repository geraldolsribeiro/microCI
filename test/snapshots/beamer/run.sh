#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" beamer snapshots/beamer/input.yml snapshots/beamer/expected.sh
