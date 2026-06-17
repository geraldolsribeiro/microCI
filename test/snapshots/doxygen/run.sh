#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" doxygen snapshots/doxygen/input.yml snapshots/doxygen/expected.sh
