#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" beamer snapshot_create_script/beamer/input.yml snapshot_create_script/beamer/expected.sh
