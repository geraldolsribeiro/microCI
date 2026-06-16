# microCI IPP image

This image supports Intel Performance Primitives (IPP) workflows in **microCI**, bundling the IPP-related dependencies needed to compile or run projects that rely on that toolkit.

## How to use with microCI

Use this image from a pipeline step and run your IPP commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Build IPP project"
    docker: "intmain/microci_ipp:0.11.0"
    plugin:
      name: bash
      bash: |
        make
```

## Related documentation

- **microCI** docs: https://microci.dev
