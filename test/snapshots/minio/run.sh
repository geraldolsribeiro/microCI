#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/run_snapshot.sh" minio snapshots/minio/input.yml snapshots/minio/expected.sh
