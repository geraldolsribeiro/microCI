# 📚 docmd plugin

## What it does

Generate Markdown documentation from source files.

## Why it exists

Use `docmd` when you want documentation to be generated from the same portable pipeline as the rest of your workflow.

## How it works

`docmd` reads source files, extracts structured documentation, and writes Markdown output.

## Example

```yaml
steps:
  - name: "Extract documentation"
    plugin:
      name: "docmd"
      toc: false
      show_details: false
      show_source: false
      show_banner: true
      items:
        - [ 'yaml', 'new/bash.yml', 'docs/plugin_bash.md' ]
        - [ 'yaml', 'new/skip.yml', 'docs/plugin_skip.md' ]
        - [ 'c++',  'input_filename.cpp', 'output_filename_cpp.md' ]
```

## Generated documentation

microCI's own documentation is generated with `intmain_docmd` from the source file below:

```bash
intmain_docmd ruby     bin/intmain_docmd               doc/236_plugin_docmd.md
intmain_docmd bash     bin/intmain_resize_screenshots  doc/110_Redimensionador_de_screenshots.md
intmain_docmd bash     scripts/Common/intmain.bash     doc/110_Biblioteca_de_scripts_intmain.md
intmain_docmd bash     scripts/Common/taoker.bash      doc/111_Biblioteca_de_scripts_taoker.md
intmain_docmd bash     scripts/Common/common.bash      doc/120_Biblioteca_de_funções_bash.md
intmain_docmd makefile scripts/Common/Makefile.include doc/130_Biblioteca_de_alvos_make.md
```
