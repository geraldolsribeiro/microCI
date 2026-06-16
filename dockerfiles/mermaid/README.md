# microCI Mermaid image

This image renders Mermaid diagrams in **microCI**, packaging the Mermaid toolchain so diagrams can be generated in a reproducible container.

## How to use with microCI

Use this image in a pipeline step and run Mermaid commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Render Mermaid diagrams"
    docker: "intmain/microci_mermaid:0.11.0"
    plugin:
      name: bash
      bash: |
        mmdc -i diagram.mmd -o diagram.svg
```

## Related documentation

- **microCI** docs: https://microci.dev
