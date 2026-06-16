# microCI DocMD image

This image exists to generate documentation from Markdown sources in **microCI**.

## How to use with microCI

Use this image in a step that runs the `docmd` plugin.

Example:

```yaml
steps:
  - name: "Generate documentation"
    docker: "intmain/microci_docmd:0.11.0"
    plugin:
      name: docmd
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/236_plugin_docmd.md`

## Default plugin

This image is used by the `docmd` plugin.
