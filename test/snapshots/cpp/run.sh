#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" cpp snapshots/cpp/input.yml snapshots/cpp/expected.sh
