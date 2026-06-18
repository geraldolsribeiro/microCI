# Runtime tests

This suite validates behavior by executing generated scripts.

Use runtime tests when:
- the output is easier to verify by side effects than by text
- the test needs to inspect artifacts, exit codes, or logs

Layout:
- `test/runtime/<plugin>_<NN>/test.sh` — execution wrapper
- `test/runtime/runner_helper.sh` — shared execution helper
- optional plugin-specific output checks inside `test.sh`

Convention:
- each plugin may have more than one runtime test folder
- use suffixes like `_01`, `_02`, ...
- `test.sh` must fail immediately if `microCI | bash` fails or times out
- after that, the developer may add custom output assertions
- the suite is scaffolded and currently skipped until concrete fixtures exist

Run with:
- `make -C test runtime_test`
