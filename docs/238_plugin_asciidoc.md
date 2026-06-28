# 📚 AsciiDoc plugin

## What it does

Convert AsciiDoc sources into HTML or other publishable formats.

## Why it exists

Use `asciidoc` when AsciiDoc documentation needs to be built as part of the same portable pipeline as the rest of your workflow.

## How it works

The plugin runs the `intmain/microci_asciidoc` container and forwards the input file(s), destination directory, and output file to the AsciiDoc toolchain.

## Example

```yaml
steps:
  - name: "Generate HTML from AsciiDoc"
    description: "Description of this step"
    plugin:
      name: asciidoc
      output_format: html
      destination_dir: _book
      output: index.html
      input:
        - book/index.adoc

  - name: "Generate PDF from AsciiDoc"
    description: "Description of this step"
    plugin:
      name: asciidoc
      output_format: pdf
      destination_dir: _book
      output: index.pdf
      input:
        - book/index.adoc
```

`output` is relative to `destination_dir`.
```

## Related image

- `dockerfiles/asciidoc/README.md`
