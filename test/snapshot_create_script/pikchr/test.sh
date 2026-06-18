#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" pikchr snapshot_create_script/pikchr/input.yml snapshot_create_script/pikchr/expected.sh
