#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" bash snapshot_create_script/bash/input.yml snapshot_create_script/bash/expected.sh
