#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" bash snapshots/bash/input.yml snapshots/bash/expected.sh
