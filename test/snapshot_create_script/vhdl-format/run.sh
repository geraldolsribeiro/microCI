#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" vhdl-format snapshot_create_script/vhdl-format/input.yml snapshot_create_script/vhdl-format/expected.sh
