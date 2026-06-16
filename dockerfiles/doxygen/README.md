# microCI Doxygen image

This image exists to generate API documentation from source code in **microCI**.

## How to use with microCI

Use this image in a step that runs the `doxygen` plugin.

Example:

```yaml
steps:
  - name: "Generate API docs"
    docker: "intmain/microci_doxygen:0.11.0"
    plugin:
      name: doxygen
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/237_plugin_doxygen.md`

## Default plugin

This image is used by the `doxygen` plugin.
