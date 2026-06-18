# Snapshot tests

This suite validates generated shell scripts.

Decision:
- compare canonicalized shell output instead of raw text
- use `shfmt` during normalization so whitespace-only edits do not fail tests
- keep `expected.sh` files machine-generated when possible
- `run_snapshot.sh` is a shared helper; call it from per-plugin wrappers only

Execution chain:
- `make -C test snapshot_create_script_test`
- `make -C test snapshot_create_script_update`
- `test/snapshot_create_script/test_all.sh`
- `test/snapshot_create_script/<plugin>/test.sh`
- `test/snapshot_create_script/runner_helper.sh`
- `test/snapshot_create_script/update_expected.sh`

Layout:
- `input.yml` — fixture used to generate the script
- `expected.sh` — golden shell output
- `run.sh` — thin wrapper that calls the shared snapshot runner
