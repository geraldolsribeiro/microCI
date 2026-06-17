#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" cpp snapshot_create_script/cpp/input.yml snapshot_create_script/cpp/expected.sh
