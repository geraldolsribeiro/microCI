# Beamer Plugin Documentation (beamer)

## 🚀 Overview
The **Beamer** plugin allows developers within the **microCI** environment to seamlessly generate professional, high-quality presentation slides directly from structured Markdown content. Designed for technical authors, researchers, and teams needing reproducible documentation output, this plugin leverages LaTeX's powerful Beamer class structure to transform markdown files into polished PDF presentations suitable for large audiences or formal reports.

Its primary function is simplifying the complex process of presentation creation by integrating it as a native step within your **microCI** CI/CD pipeline.

## ✨ Features
### Produce Great Looking Presentations using Markdown
This core feature transforms plain Markdown syntax (which must be structured according to Beamer's conventions) into fully typeset LaTeX Beamer slides, resulting in a professional PDF output. Users benefit from:
*   **Structured Templating:** Effortlessly apply consistent styling (themes, color schemes, etc.) across all slides without manual compilation effort.
*   **Source Control Integration:** Since the content lives alongside the CI configuration (`.microCI.yml`), presentation source files are version-controlled and reproducible builds are guaranteed.
*   **Modular Content Generation:** The ability to list multiple source markdown files (`source: []`) allows for breaking up a large presentation into manageable, independently editable modules (e.g., introduction, body, conclusion).

## ⚙️ Setup & Configuration
To utilize the Beamer plugin, you must specify the core parameters in your `.microCI.yml` file under the `plugin:` block. The plugin requires access to all source markdown files and defines the final output metadata for the presentation.

### Parameters

| Parameter | Type | Description | Required? | Notes |
| :--- | :--- | :--- | :--- | :--- |
| `name` | String | The name of the step in the pipeline. | Yes | Used for reporting purposes within **microCI**. |
| `output` | String | The filename for the resulting PDF presentation (e.g., `presentation.pdf`). | Yes | This determines the output artifact name. |
| `source` | List[String] | A list of paths to the source Markdown files that contain slide content. | Yes | Files should be ordered logically, especially if using section numbering schemes like `01-`. |
| `lang` | String | The language used in the presentation (e.g., `en-US`, `pt-BR`). | No | Influences date formatting and language packages. |
| `title` | String | The main title displayed on the presentation's title slide. | Recommended | Mandatory for a complete title slide. |
| `subtitle` | String | A secondary subtitle for the presentation. | Optional | Provides context to the title. |
| `author` | List[String] | A list of names credited as authors. | Recommended | Format should be a list of strings. |
| `institute` | String | The name of the presenting institution/company. | Optional | Appears in the header or footer metadata. |
| `date` | String / Date | The date associated with the presentation (e.g., `\today`). | Optional | Can use LaTeX commands for dynamic dates. |
| **Advanced** | | | | |
| `theme` | String | Specifies the Beamer theme to be used (e.g., `Madrid`, `Boadilla`). | No | Overrides default styling. Check Beamer documentation for available themes. |
| `header-includes`| List[String] | A list of raw LaTeX packages or commands to include at the start of the document (`\usepackage{...}`). | Optional | Use this for advanced features like complex math or TikZ graphics. |

## 💻 Examples
The following example demonstrates how to use the **Beamer** plugin within a typical **microCI** workflow to build a multi-section presentation PDF.

### Configuration File: `.microCI.yml` (Excerpt)

```yaml
steps:
  - name: Build Presentation Slides
    plugin:
      name: beamer
      output: final_tech_talk.pdf
      source: # List of source files, use the number for ordering the sections
        - 00-introduction.md
        - 01-body.md
        - 02-conclusion.md
        - 99-references.md
      lang: en-US # Presentation language: pt-BR, en-US, ...
      institute: Tech Solutions Corp
      date: April 1, 2023
      title: Mastering MicroCI Plugins
      subtitle: A Technical Deep Dive into CI/CD Automation
      author:
        - Geraldo Ribeiro
        - AI Assistant Team
      header-includes: # Custom LaTeX package inclusion
        - \usepackage{tikz}
```

### Source File Example (`01-body.md`)
*Content for the `source` files must be structured in Markdown, but will be processed by Beamer's underlying LaTeX engine.*

````markdown
# Section 1: Plugin Architecture {#section-plugin-architecture}

This is the main content of our body section. Every top-level heading (`#`) or second-level heading (`##`) defined within the source files becomes a slide in the final PDF.

## Key Components

The plugin architecture consists of three parts:
1. The **microCI** Runner (Execution Engine).
2. The Plugin Core (Logic layer).
3. The Beamer Renderer (Output formatter).

We are demonstrating how easy it is to integrate complex tooling into a simple CI/CD workflow.
````

***
*This documentation was generated by the Documentation Generator Skill.*