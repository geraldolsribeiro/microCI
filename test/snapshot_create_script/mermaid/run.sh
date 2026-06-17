#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" mermaid snapshot_create_script/mermaid/input.yml snapshot_create_script/mermaid/expected.sh
