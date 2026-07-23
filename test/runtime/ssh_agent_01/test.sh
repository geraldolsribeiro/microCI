#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

workdir="$(mktemp -d)"
home_dir="$workdir/home"
mkdir -p "$home_dir/.ssh"
trap 'rm -rf "$workdir"' EXIT

cp "$script_dir/.microCI.yml" "$workdir/.microCI.yml"

# Create a passphrase-protected key that microCI should unlock via ssh-agent.
ssh-keygen -t ed25519 -N "secret" -f "$home_dir/.ssh/id_ed25519" >/dev/null
: >"$home_dir/.ssh/config"
: >"$home_dir/.ssh/known_hosts"

# Expected behavior after the fix:
# - microCI asks for/unlocks the key
# - ssh-agent is started automatically
# - the container can see SSH_AUTH_SOCK and list the loaded key
if ! env HOME="$home_dir" bash -lc "cd '$workdir' && '$repo_root/bin/microCI' | bash" >"$workdir/output.txt" 2>&1; then
  echo "[runtime] FAIL  ssh_agent_01: microCI could not use a passphrase-protected key"
  cat "$workdir/output.txt"
  exit 1
fi

# The container should have access to the agent and print a public key.
grep -q "ssh-ed25519" "$workdir/output.txt"

echo "[runtime] PASS  ssh_agent_01"
