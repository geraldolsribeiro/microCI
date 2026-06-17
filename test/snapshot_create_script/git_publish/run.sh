#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" git_publish snapshot_create_script/git_publish/input.yml snapshot_create_script/git_publish/expected.sh
