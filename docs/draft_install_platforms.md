# Draft notes for install platforms

## Linux

Download the latest binary from GitHub releases:

```bash
sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/bin/microCI
sudo chmod 755 /usr/bin/microCI
```

You can also install a packaged release:

* Debian / Ubuntu: `.deb`
* Fedora / openSUSE: `.rpm`

## macOS

```bash
brew install geraldolsribeiro/tap/microci
```

## G-Tools

G-Tools can detect your system and install microCI for you:

```bash
cargo install g-tools
G microci install
```
