#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" clang-format snapshots/clang-format/input.yml snapshots/clang-format/expected.sh
