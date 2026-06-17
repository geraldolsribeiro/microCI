#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" mermaid snapshots/mermaid/input.yml snapshots/mermaid/expected.sh
