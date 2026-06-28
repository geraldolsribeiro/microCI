# microCI AsciiDoc image

This image converts AsciiDoc documentation in **microCI**, packaging the AsciiDoc toolchain so documents can be rendered in a reproducible container.

## How to use with microCI

Use this image in a step that runs the `asciidoc` plugin.

Example:

```yaml
steps:
  - name: "Generate HTML from Asciidoc"
    description: "Description of this step"
    plugin:
      name: asciidoc
      output_format: html
      destination_dir: _book
      input:
        - book/index.adoc

  - name: "Generate PDF from Asciidoc"
    description: "Description of this step"
    plugin:
      name: asciidoc
      output_format: pdf
      destination_dir: _book
      input:
        - book/index.adoc
```

This example matches the structure used in `docs/238_plugin_asciidoc.md` and only adds the container image required for the packaged toolchain.

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/238_plugin_asciidoc.md`

## Default plugin

This image is used by the `asciidoc` plugin.
