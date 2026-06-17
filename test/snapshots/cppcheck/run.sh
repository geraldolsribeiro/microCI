#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" cppcheck snapshots/cppcheck/input.yml snapshots/cppcheck/expected.sh
