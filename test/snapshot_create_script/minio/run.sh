#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/snapshot_run.sh" minio snapshot_create_script/minio/input.yml snapshot_create_script/minio/expected.sh
