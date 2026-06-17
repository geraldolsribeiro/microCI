#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" git_publish snapshots/git_publish/input.yml snapshots/git_publish/expected.sh
