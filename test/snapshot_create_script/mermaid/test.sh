#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" mermaid snapshot_create_script/mermaid/input.yml snapshot_create_script/mermaid/expected.sh
