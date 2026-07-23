#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"

workdir="$(mktemp -d)"
home_dir="$workdir/home"
mkdir -p "$home_dir/.ssh"
trap 'rm -rf "$workdir"' EXIT

cp "$script_dir/.microCI.yml" "$workdir/.microCI.yml"

# Create a password-protected SSH key pair in the fake HOME.
ssh-keygen -t ed25519 -N "secret" -f "$home_dir/.ssh/id_ed25519" >/dev/null
cp "$home_dir/.ssh/id_ed25519.pub" "$home_dir/.ssh/id_rsa.pub"
: >"$home_dir/.ssh/config"
: >"$home_dir/.ssh/known_hosts"

# microCI should reject passphrase-protected keys before running any step.
if env HOME="$home_dir" bash -c "cd '$workdir' && '$repo_root/bin/microCI' | bash" >"$workdir/output.txt" 2>&1; then
  echo "[runtime] FAIL  ssh_password_01: microCI accepted a password-protected key"
  exit 1
fi

grep -q "Please use an SSH key not protected by password" "$workdir/output.txt"
! grep -q "should not run" "$workdir/output.txt"

echo "[runtime] PASS  ssh_password_01"
