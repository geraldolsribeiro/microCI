#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

# Helper:
# 1. Create an isolated temp workspace.
# 2. Copy a minimal .microCI.yml into it.
# 3. Write a test-specific .env file.
# 4. Generate the bash script with microCI.
# 5. Compare only the env-related lines against a reference file.
# 6. Optionally assert that forbidden strings never appear anywhere else.
run_case() {
  local name="$1"
  local env_content="$2"
  local env_name="$3"
  local expected_ref="$4"
  local forbidden_pattern="${5:-}"
  local tmpdir

  # Use a fresh directory so each case is fully independent.
  tmpdir="$(mktemp -d)"
  trap 'rm -rf "$tmpdir"' RETURN

  # Minimal pipeline config for this fixture.
  cp "$script_dir/.microCI.yml" "$tmpdir/.microCI.yml"

  # Inject the exact .env content we want to validate.
  printf '%s' "$env_content" >"$tmpdir/.env"

  # Generate the shell script that microCI would execute.
  (cd "$tmpdir" && "$repo_root/bin/microCI") >"$tmpdir/generated.sh"

  # Pull out just the env declaration lines we care about.
  grep -E "^  local ${env_name}=|--env ${env_name}=" "$tmpdir/generated.sh" >"$tmpdir/actual.ref"

  # Exact diff against the expected reference.
  if ! diff -u "$expected_ref" "$tmpdir/actual.ref"; then
    echo "[runtime] FAIL  env_file_01:$name"
    exit 1
  fi

  # Negative check: ensure ignored/malformed fragments never appear.
  if [[ -n "$forbidden_pattern" ]] && grep -Eq "$forbidden_pattern" "$tmpdir/generated.sh"; then
    echo "[runtime] FAIL  env_file_01:$name (forbidden pattern matched: $forbidden_pattern)"
    exit 1
  fi

  rm -rf "$tmpdir"
  trap - RETURN
}

# Valid comments must be ignored; FOO must still be captured.
run_case "comments_only" $'# comment\n   # indented comment\nFOO=bar\n' "FOO" "$script_dir/ref_comments_only.txt" '# comment|indented comment'

# Empty lines must be ignored; FOO must still be captured.
run_case "blank_lines" $'\n\nFOO=bar\n' "FOO" "$script_dir/ref_blank_lines.txt"

# Malformed lines must be ignored; only NAME=ok should survive.
run_case "malformed_lines" $'NO_EQUALS\n=missing_name\nNAME=ok\n' "NAME" "$script_dir/ref_malformed_lines.txt" 'NO_EQUALS|missing_name|=missing_name'

# Whitespace-only lines must be ignored; X must still be captured.
run_case "whitespace_only" $'   \n\t\nX=1\n' "X" "$script_dir/ref_whitespace_only.txt"

# A valid key/value with leading spaces in the file must still be captured.
run_case "trailing_spaces" $'A=1\n  B = two\n' "A" "$script_dir/ref_trailing_spaces.txt" 'B = two'

echo "[runtime] PASS  env_file_01"
