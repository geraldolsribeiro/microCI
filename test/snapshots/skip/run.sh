#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" skip snapshots/skip/input.yml snapshots/skip/expected.sh
