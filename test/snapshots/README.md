# Snapshot tests

This suite validates generated shell scripts.

Decision:
- compare canonicalized shell output instead of raw text
- use `shfmt` during normalization so whitespace-only edits do not fail tests
- keep `expected.sh` files machine-generated when possible
- `run_snapshot.sh` is a shared helper; call it from per-plugin wrappers only

Execution chain:
- `make -C test snapshot_test`
- `test/snapshots/snapshot_test.sh`
- `test/snapshots/<plugin>/run.sh`
- `test/snapshots/run_snapshot.sh`

Layout:
- `input.yml` — fixture used to generate the script
- `expected.sh` — golden shell output
- `run.sh` — thin wrapper that calls the shared snapshot runner
