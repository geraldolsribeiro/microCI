# Installing microCI 

If you're running macOS, Linux, or another Unix-like operating system, you can
install **microCI** by running the following command in your terminal:

<center>![Install](images/microci-install.svg)</center>

```bash
curl -fsSL https://microci.dev/install.sh | bash
```

## Alternative installation methods

### Install on Linux

To install the latest stable version on Linux:

#### Binary

```bash
sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/bin/microCI
sudo chmod 755 /usr/bin/microCI
```


#### Debian/Ubuntu

* Download [microci_0.44.0_amd64.deb](https://github.com/geraldolsribeiro/microCI/releases/download/latest/microci_0.44.0_amd64.deb)
  from [github releases](https://github.com/geraldolsribeiro/microCI/releases):

#### Fedora/openSUSE

* Download [microci-0.44.0-2.x86_64.rpm](https://github.com/geraldolsribeiro/microCI/releases/download/latest/microci-0.44.0-2.x86_64.rpm)
  from [github releases](https://github.com/geraldolsribeiro/microCI/releases):

### Install on macOS

To install the latest stable version on macOS:

```bash
brew install geraldolsribeiro/tap/microci
```

### Install using G-Tools

**G-Tools** automatically detects your OS and installs **microCI** accordingly.

```bash
cargo install g-tools
G microci install
```
