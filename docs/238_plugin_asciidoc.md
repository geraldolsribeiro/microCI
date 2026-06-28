# 📚 asciidoc plugin

## What it does

Convert AsciiDoc sources into HTML or other publishable formats.

## Why it exists

Use `asciidoc` when documentation needs to be built as part of the same portable pipeline as the rest of your workflow.

## How it works

The plugin runs the `intmain/microci_asciidoc` container and forwards the input file(s), options, and output file to the AsciiDoc toolchain.

## Example

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

## Related image

- `dockerfiles/asciidoc/README.md`
