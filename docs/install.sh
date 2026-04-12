#!/usr/bin/env bash
set -e

OS=$(uname -s)

case "$OS" in
"Linux")
  command -v curl &>/dev/null ||
    {
      echo -e "{{RED}}The utility curl was not found in the system{{CLEAR}}"
      sudo apt update
      sudo apt install -y curl
    }
  sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI -o /usr/bin/microCI
  sudo chmod 755 /usr/bin/microCI
  ;;
"Darwin")
  brew install geraldolsribeiro/tap/microci
  ;;
*)
  echo "Running on an unknown operating system: $OS"
  exit 1
  ;;
esac

echo "✅ MicroCI installed successfully!"
microCI --version
