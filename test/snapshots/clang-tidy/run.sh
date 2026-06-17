#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" clang-tidy snapshots/clang-tidy/input.yml snapshots/clang-tidy/expected.sh
