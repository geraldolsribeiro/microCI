# `microCI --new` tests

This suite validates the `microCI --new <plugin>` templates.

Layout:
- `snapshot_cmd_new/<plugin>/expected.yml` — canonical generated fixture
- `snapshot_cmd_new/<plugin>/run.sh` — wrapper that calls the shared runner
- `snapshot_cmd_new/snapshot_run.sh` — shared comparison helper

The test tree is auto-discovered, so adding a new plugin only requires adding a
new directory under `test/snapshot_cmd_new/<plugin>/`.

Status convention:
- `run.sh` may print `SKIP` for fixtures that are not ready yet
- the aggregator counts `PASS`, `FAIL`, and `SKIP` and prints a summary

Run with:
- `make -C test snapshot_cmd_new_test`
