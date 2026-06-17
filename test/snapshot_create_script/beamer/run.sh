#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" beamer snapshot_create_script/beamer/input.yml snapshot_create_script/beamer/expected.sh
