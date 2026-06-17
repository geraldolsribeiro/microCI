#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" flawfinder snapshots/flawfinder/input.yml snapshots/flawfinder/expected.sh
