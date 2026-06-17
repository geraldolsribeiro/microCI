#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" docmd snapshots/docmd/input.yml snapshots/docmd/expected.sh
