#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" npm snapshots/npm/input.yml snapshots/npm/expected.sh
