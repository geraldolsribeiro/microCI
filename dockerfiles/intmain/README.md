# microCI Utils image

This image provides a small utility, C++ build, testing, and packaging environment for **microCI** workflows, with helper tools used by pipeline steps that need a minimal but repeatable execution environment.

## How to use with microCI

Use this image from a pipeline step and run the required commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Build C++ code"
    docker: "intmain/microci_utils:0.11.0"
    plugin:
      name: bash
      bash: |
        g++ --version
        make
```

## Included tools

### Debian packages
- bash
- build-essential
- fontconfig
- jq
- plantuml
- ttf-mscorefonts-installer
- xmlstarlet

### npm packages
- @mryhryki/markdown-preview
- coffeescript
- coffeescript-concat
- codeceptjs
- compass-sass-mixins
- css-combine
- htmllint
- htmllint-cli
- http-server
- js-beautify
- js2coffee
- jshint
- sass
- sass-migrator
- svgo
- terser
- vue-cli
- web-push
- yarn

### Ruby gems
- cucumber
- html-proofer
- mdl
- syntax

## Related documentation

- **microCI** docs: https://microci.dev
