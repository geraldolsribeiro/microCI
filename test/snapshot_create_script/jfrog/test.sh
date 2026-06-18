#!/usr/bin/env bash

"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" jfrog snapshot_create_script/jfrog/input.yml snapshot_create_script/jfrog/expected.sh
