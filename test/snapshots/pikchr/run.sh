#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" pikchr snapshots/pikchr/input.yml snapshots/pikchr/expected.sh
