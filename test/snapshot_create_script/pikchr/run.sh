#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" pikchr snapshot_create_script/pikchr/input.yml snapshot_create_script/pikchr/expected.sh
