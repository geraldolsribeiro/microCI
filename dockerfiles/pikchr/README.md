# microCI Pikchr image

This image renders Pikchr diagrams in **microCI**, packaging the Pikchr renderer so diagram generation can run in a reproducible container.

## How to use with microCI

Use this image in a step that runs the `pikchr` plugin.

Example:

```yaml
steps:
  - name: "Render Pikchr diagram"
    docker: "intmain/microci_pikchr:0.11.0"
    plugin:
      name: pikchr
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/235_plugin_pikchr.md`

## Default plugin

This image is used by the `pikchr` plugin.
