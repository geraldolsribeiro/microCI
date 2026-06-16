# microCI Poppler image

This image supports PDF and document-processing workflows in **microCI**, packaging Poppler-related tools so document conversion and inspection tasks can run in a reproducible container.

## How to use with microCI

Use this image from a pipeline step and run the Poppler command you need in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Inspect PDF artifacts"
    docker: "intmain/microci_poppler:0.11.0"
    plugin:
      name: bash
      bash: |
        pdfinfo input.pdf
```

## Related documentation

- **microCI** docs: https://microci.dev
