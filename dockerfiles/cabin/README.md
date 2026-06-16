# microCI CABIN image

This image supports **microCI** steps that need the CABIN toolchain, bundling the dependencies required by CABIN-related workflows so they can run in a reproducible container.

## How to use with microCI

Use this image from a pipeline step and run your CABIN commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Run CABIN task"
    docker: "intmain/microci_cabin:0.11.0"
    plugin:
      name: bash
      bash: |
        cabin --help
```

## Related documentation

- **microCI** docs: https://microci.dev
