#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" vhdl-format snapshot_create_script/vhdl-format/input.yml snapshot_create_script/vhdl-format/expected.sh
