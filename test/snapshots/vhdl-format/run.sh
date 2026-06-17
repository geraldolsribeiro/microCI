#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" vhdl-format snapshots/vhdl-format/input.yml snapshots/vhdl-format/expected.sh
