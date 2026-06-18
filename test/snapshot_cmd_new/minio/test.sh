#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" "minio" "snapshot_cmd_new/minio/expected.yml"
