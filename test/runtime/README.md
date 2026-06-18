# Runtime tests

This suite validates behavior by executing generated scripts.

Use runtime tests when:
- the output is easier to verify by side effects than by text
- the test needs to inspect artifacts, exit codes, or logs

Layout:
- `test/runtime/<plugin>_<NN>/test.sh` — execution wrapper
- `test/runtime/runner_helper.sh` — shared execution helper
- optional plugin-specific output checks inside `test.sh`

Pattern:
- `test.sh` defines a `run_microci()` function for its own command line
- `runner_helper.sh` runs that command and handles timeout cleanup
- `verify_runtime_output()` contains the developer-defined assertions
- use `return` inside helper functions and `exit` in top-level flow
- future improvements may factor shared assertions into reusable helpers

Convention:
- each plugin may have more than one runtime test folder
- use suffixes like `_01`, `_02`, ...
- each `test.sh` runs from its own folder
- tests that expect `microCI` to fail must declare that expectation explicitly (see `expect_fail`)
- tests that expect success should fail immediately if `microCI | bash` fails or times out
- after that, the developer may add custom output assertions
- the suite is scaffolded and currently skipped until concrete fixtures exist

Expected-failure policy:
- place an `expect_fail` marker file in the test folder to mark failure-oriented tests
- `test_all.sh` reports `PASS` when the outcome matches the expectation (success or expected failure)
- `test_all.sh` reports `FAIL` when the outcome does not match the expectation
- future improvement: replace the marker file with a richer metadata file if needed

Skip policy:
- runtime tests are skipped in `test_all.sh` by name pattern
- current skipped prefixes: `beamer_`, `docmd_`, `doxygen_`, `jfrog_`, `minio_`, `npm_`, `mermaid_`, `pikchr_`, `vhdl-format_`
- future improvement: store skip status next to each fixture instead of hard-coding names

Timeout control:
- `runner_helper.sh` accepts an optional timeout argument
- `test.sh` passes `${RUNTIME_TIMEOUT:-120s}` by default
- override it when needed, e.g. `RUNTIME_TIMEOUT=300s make -C test runtime_test`

Run with:
- `make -C test runtime_test`
