#!/usr/bin/env bash
set -e

OS=$(uname -s)

case "$OS" in
  Linux)
    if ! command -v curl &>/dev/null; then
      echo -e "{{RED}}The 'curl' utility was not found in the system.{{CLEAR}}"
      sudo apt update
      sudo apt install -y curl
    fi
    sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI -o /usr/bin/microCI
    sudo chmod 755 /usr/bin/microCI
    ;;
  Darwin)
    brew install geraldolsribeiro/tap/microci
    ;;
  *)
    echo "Error: Unsupported operating system: $OS"
    exit 1
    ;;
esac

echo "✅ microCI installed successfully!"
microCI --version
