#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" git_deploy snapshots/git_deploy/input.yml snapshots/git_deploy/expected.sh
