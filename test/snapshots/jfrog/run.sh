#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" jfrog snapshots/jfrog/input.yml snapshots/jfrog/expected.sh
