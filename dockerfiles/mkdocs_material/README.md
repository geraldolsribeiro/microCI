# microCI MkDocs Material image

This image builds MkDocs Material sites in **microCI**, bundling MkDocs Material and related tooling so documentation sites can be generated consistently in a container.

## How to use with microCI

Use this image in a step that runs the `mkdocs_material` plugin.

Example:

```yaml
steps:
  - name: "Build documentation site"
    docker: "intmain/microci_mkdocs_material:0.11.0"
    plugin:
      name: mkdocs_material
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/230_plugin_mkdocs_material.md`

## Default plugin

This image is used by the `mkdocs_material` plugin.
