# microCI Pandoc image

This image converts documentation and markup formats in **microCI**, bundling Pandoc so pipelines can transform Markdown, HTML, and other supported formats in a reproducible container.

## How to use with microCI

Use this image in a step that runs the `pandoc` plugin.

Example:

```yaml
steps:
  - name: "Convert documentation"
    docker: "intmain/microci_pandoc:0.11.0"
    plugin:
      name: pandoc
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/232_plugin_pandoc.md`

## Default plugin

This image is used by the `pandoc` plugin.
