# Installation

It looks like you’re running macOS, Linux, or another Unix-like OS. To download
and install **microCI**, run the following in your terminal, then follow the
on-screen instructions.

<center>![Install](images/microci-install.svg)</center>

```bash
curl -fsSL https://microci.dev/install.sh | bash
```

## Alternative ways to install

### Install on Linux

To install the latest stable version run on Linux:

```bash
sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/bin/microCI
sudo chmod 755 /usr/bin/microCI
```

### Install on MacOS with brew

To install the latest stable version run on MacOS:

```bash
brew install geraldolsribeiro/tap/microci
```

### Install using G-Tools

**G-Tools** install **microCI** based on your OS.

```bash
cargo install g-tools
G microci install
```
