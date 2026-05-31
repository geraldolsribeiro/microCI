# pandoc plugin

## Overview

The **microCI** `pandoc` plugin converts document sources into other formats during a pipeline run. It is useful for generating PDFs, HTML, DOCX, EPUB, or other Pandoc-supported outputs from Markdown or similar markup files as part of documentation, release, or publishing workflows.

## Features

- **Document format conversion**
  - Converts one or more input files into a target output format.
  - Use case: publish a `README.md` as `README.pdf` in CI.

## Setup & Configuration

1. Add a pipeline step to `.microCI.yml`.
2. Set the plugin name to `pandoc`.
3. Define the input file(s) to convert.
4. Choose the output filename and desired Pandoc options.
5. Run the pipeline so **microCI** executes the conversion step.

### Configuration reference

| Parameter | Type | Required | Description | Example |
|---|---:|---:|---|---|
| `name` | string | yes | Plugin identifier. | `pandoc` |
| `base_path` | string | no | Working directory used to resolve input files. | `.` |
| `inputs` | array[string] | yes | Source document(s) to convert. | `['README.md']` |
| `output` | string | yes | Output file to generate. | `README.pdf` |
| `options` | array[string] | no | Additional Pandoc CLI options passed to the converter. | `['--standalone', '--toc', '--number-sections']` |

### Example configuration

Source example from `new/pandoc.yml`:

```yaml
steps:
  - name: "Generate PDF from Markdown"
    description: "Description of this step"
    plugin:
      name: pandoc
      base_path: .
      inputs:
        - README.md
      output: README.pdf
      options:
        - --standalone
        - --toc
        - --number-sections
```

## Examples

### Convert `README.md` to PDF

```yaml
steps:
  - name: "Generate PDF from Markdown"
    description: "Build a PDF artifact from the project README"
    plugin:
      name: pandoc
      base_path: .
      inputs:
        - README.md
      output: README.pdf
      options:
        - --standalone
        - --toc
        - --number-sections
```

This step reads `README.md` from the configured `base_path`, converts it with Pandoc, and writes the resulting PDF to `README.pdf`.
