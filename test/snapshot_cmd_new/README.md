# `microCI --new` tests

This suite validates the `microCI --new <plugin>` templates.

Layout:
- `test/snapshot_cmd_new/<plugin>/expected.yml` — canonical generated fixture
- `test/snapshot_cmd_new/<plugin>/test.sh` — wrapper that calls the shared runner
- `test/snapshot_cmd_new/runner_helper.sh` — shared comparison helper

The test tree is auto-discovered, so adding a new plugin only requires adding a
new directory under `test/snapshot_cmd_new/<plugin>/`.

Status convention:
- `test.sh` may print `SKIP` for fixtures that are not ready yet
- the aggregator counts `PASS`, `FAIL`, and `SKIP` and prints a summary

Run with:
- `make -C test snapshot_cmd_new_test`

JUnit output:
- `test/snapshot_cmd_new/snapshot-cmd-new-junit.xml`
