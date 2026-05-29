# 📑 The **docmd** Plugin

## Overview

The **docmd** plugin is a powerful utility designed to streamline and automate the process of generating comprehensive technical documentation directly from your project's source code base. Instead of relying on manual doc comments or scattered README files, **docmd** analyzes specified source directories and extracts structured information—such as function signatures, class definitions, and inline descriptions—and transforms it into clean, maintainable Markdown documents.

This plugin is essential for any project aiming to keep its documentation synchronized with its codebase, significantly reducing technical debt and improving the developer onboarding experience within a **microCI** environment.

## Features

The core capability of this plugin is simple yet profound: it extracts documentation from source code to markdown files.

### 📚 Source Code Documentation Extraction
**Description:** This feature analyzes predefined directories (e.g., `src/`, `lib/`) and identifies code constructs that contain doc comments or specific metadata tags. It then parses these structures, sanitizes them, and formats the resulting information into structured Markdown files. The plugin supports various languages (YAML, C++, Python, Rust, etc.) for extraction.
**Use Case:** Ideal for large codebases where manual documentation is tedious. You can enforce a standard level of documentation coverage across all modules simply by integrating **docmd** into your build pipeline and defining the required language formats.

## Setup & Configuration

### ⚙️ Installation

The `docmd` plugin should be added to your main CI/CD configuration file, typically `.microCI.yml`. No external dependencies beyond the core **microCI** runner are generally required, assuming source code analysis tools are available in the execution environment path and language-specific parsers are configured.

### 📄 Configuration Parameters

The primary configuration is handled within the `plugin` block of your workflow step. The plugin expects input parameters specifying what to analyze and where to place the output.

| Parameter | Type | Description | Required? | Example Value |
| :--- | :--- | :--- | :--- | :--- |
| `toc` | Boolean | Controls whether a Table of Contents is generated in the markdown file. | No | `false` |
| `show_details` | Boolean | Enables/disables showing detailed HTML tags within the documentation output. | No | `false` |
| `show_source` | Boolean | Controls whether source filenames should be printed alongside documentation blocks. | No | `false` |
| `show_banner` | Boolean | Enables/disables a header banner on the generated document. | No | `true` |
| `items` | Array<String> | A list of language-specific items to process, detailing paths and desired output filenames. | Yes | `['yaml', 'new/bash.yml', 'docs/plugin_bash.md']` |

### Example Configuration (`.microCI.yml`)

To enable and configure **docmd** for a multi-format project setup:

```yaml
steps:
    # The plugin processes multiple items, each defined by language, source path, and output file name.
    - name: "Extract comprehensive documentation"
      plugin:
        name: "docmd"
        toc: false          # Disable Table of Contents generation for cleaner API docs
        show_details: false 
        show_source: true   
        show_banner: true   
        items:
          - [ 'yaml', 'new/bash.yml', 'docs/plugin_bash.md' ] # Processes YAML files
          - [ 'c++',  'input_filename.cpp', 'output_filename_cpp.md' ] # Processes C++ headers
```

## Examples

This section demonstrates how to integrate **docmd** into your CI/CD pipeline using the standard `.microCI.yml` structure, utilizing multiple language parsers and specifying outputs per file type.

```yaml
steps:
  - name: "Extract documentation"
    description: "Extract documentation from source code to a markdown file"
    plugin:
      name: "docmd"
      toc: false          # Enable/Disable table of content
      show_details: false # Enable/Disable html tag details
      show_source: false  # Enable/Disable source filename print
      show_banner: true   # Enable/Disable banner
      items:
        - [ 'yaml', 'new/bash.yml', 'docs/plugin_bash.md' ]
        - [ 'yaml', 'new/skip.yml', 'docs/plugin_skip.md' ]
        - [ 'c++',  'input_filename.cpp', 'output_filename_cpp.md' ]
        # Available formats:
        # `bash`, `c++`, `coffee`, `conf`, `grdb`, `makefile`, `nginx`, `python`, `ruby`, `rust`, `scss`, `systemd`, `vim`, `yaml`
```