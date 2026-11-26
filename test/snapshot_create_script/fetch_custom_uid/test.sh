#!/usr/bin/env bash
"$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/runner_helper.sh" fetch snapshot_create_script/fetch_custom_uid/input.yml snapshot_create_script/fetch_custom_uid/expected.sh
