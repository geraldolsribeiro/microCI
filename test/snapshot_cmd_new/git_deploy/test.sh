#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "git_deploy" "snapshot_cmd_new/git_deploy/expected.yml"
