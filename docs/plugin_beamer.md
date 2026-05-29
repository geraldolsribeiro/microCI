# 🎓 Beamer Plugin Documentation

The `beamer` plugin is a specialized tool that allows users to generate professionally structured and aesthetically pleasing presentations directly from source Markdown files within the **microCI** workflow. By leveraging LaTeX compilation (specifically, the Beamer class), it ensures high fidelity and consistent visual design across all generated slides.

## Overview
Beamer provides a robust mechanism for turning multi-file markdown content into printable PDF slide decks suitable for academic talks, corporate pitches, or educational materials. Instead of relying on basic image embedding, this plugin uses the power of LaTeX to manage layout, typography, and structure, resulting in professional-grade presentations that are guaranteed to compile correctly.

A user should employ this plugin any time a formal, visually polished presentation needs to be generated as part of an automated build or deployment pipeline within **microCI**.

## ✨ Features
The `beamer` plugin excels at streamlining the conversion process:

*   **Produce Great Looking Presentations using Markdown:** This core feature allows developers to write content using simple markdown syntax (e.g., headers for sections, lists for bullet points) while retaining all the structural advantages of a powerful typesetting system like LaTeX/Beamer.
    *   ***Use Case:*** Quickly prototyping slide decks where complex styling or academic formatting is required, without manually managing LaTeX source files.

## ⚙️ Setup & Configuration
The Beamer plugin processes multiple markdown sources into a single compiled output (PDF). Configuration involves defining the input structure and specifying metadata.

### Parameters Reference

| Parameter | Type | Default Value | Description | Required? |
| :--- | :--- | :--- | :--- | :--- |
| `output` | String | N/A | The desired filename for the compiled presentation (e.g., `report.pdf`). | Yes |
| `source` | List<Path> | N/A | A list of markdown files (`*.md`) that contain content for the slides. Must be provided in logical order. | Yes |
| `lang` | String | N/A | The language identifier for localized content (e.g., `en-US`, `pt-BR`). | Recommended |
| `title` | String | N/A | The main title that will appear on the presentation's cover slide. | No |
| `subtitle` | String | N/A | A secondary, descriptive subtitle for the presentation. | No |
| `author` | List<String> | N/A | One or more authors responsible for the content. Can include names and affiliations. | No |
| `institute` | String | N/A | The institution or organization affiliation displayed on the title page. | No |
| `date` | String | Current Date | The date to be displayed on the presentation's cover slide (e.g., "April 2023"). | No |
| `theme` | String | Default | Specifies the LaTeX Beamer theme to use (e.g., `Madrid`, `Boadilla`). | No |
| `header-includes` | List<String> | [] | Custom raw LaTeX commands or packages that need to be included in the preamble (`\usepackage{...}`). | Optional |

### Example Configuration (`new/beamer.yml`)

```yaml
steps:
  - name: Create PDF presentation from Markdown
    plugin:
      name: beamer
      output: output_filename.pdf
      source:
        - 00-introduction.md
        - 01-body.md
        - 02-conclusion.md
        - 99-references.md
      lang: pt-BR # Set locale for proper translation of date/titles
      institute: Institution Name
      date: April 1, 2023
      title: Presentation Title
      subtitle: Presentation Subtitle
      author:
        - Geraldo Ribeiro
        - Other Author Name
      # Include custom packages needed by the slides
      header-includes:
        - \usepackage{tikz}
```

## 🚀 Examples
This example demonstrates integrating the Beamer plugin into a **microCI** workflow file (`.microCI.yml`), ensuring the entire presentation build is executed in sequence.

### Usage in CI/CD Pipeline YAML (.microCI.yml)

Place this snippet within your main workflow file:

```yaml
steps:
    - name: "Compile Documentation Presentation"
      plugin:
        name: beamer
        output: final_report.pdf
        source:
          - docs/slides/part1.md
          - docs/slides/part2.md
        lang: en-US
        title: Project Status Review
        subtitle: Q2 2026 Build Report
        author:
            - Jane Doe
            - John Smith
```

### Output Verification
Upon successful execution in **microCI**, the compiled PDF presentation (`final_report.pdf`) will be generated and made available as a build artifact, ready for download or deployment.