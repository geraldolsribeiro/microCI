#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" template snapshot_create_script/template/input.yml snapshot_create_script/template/expected.sh
