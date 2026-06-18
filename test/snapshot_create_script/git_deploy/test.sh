#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" git_deploy snapshot_create_script/git_deploy/input.yml snapshot_create_script/git_deploy/expected.sh
