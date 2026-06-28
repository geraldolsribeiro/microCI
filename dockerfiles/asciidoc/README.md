# microCI AsciiDoc image

This image converts AsciiDoc documentation in **microCI**, packaging the AsciiDoc toolchain so documents can be rendered in a reproducible container.

## How to use with microCI

Use this image in a step that runs the `asciidoc` plugin.

Example:

```yaml
steps:
  - name: "Generate HTML from AsciiDoc"
    description: "Description of this step"
    docker: "intmain/microci_asciidoc:0.11.0"
    plugin:
      name: asciidoc
      output_format: html
      destination_dir: _book
      output: index.html
      input:
        - book/index.adoc

  - name: "Generate PDF from AsciiDoc"
    description: "Description of this step"
    docker: "intmain/microci_asciidoc:0.11.0"
    plugin:
      name: asciidoc
      output_format: pdf
      destination_dir: _book
      output: index.pdf
      input:
        - book/index.adoc
```

`output` is relative to `destination_dir`.

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/238_plugin_asciidoc.md`

## Default plugin

This image is used by the `asciidoc` plugin.
