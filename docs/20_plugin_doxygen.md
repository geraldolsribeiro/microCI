# 📖 Doxygen Plugin Documentation

The Doxygen plugin integrates industry-standard documentation generation into your microCI pipeline. It allows developers to automatically generate comprehensive technical documentation from source code comments, ensuring that APIs and internal components remain properly documented as the codebase evolves.

## Overview
Doxygen is a versatile tool used by developers to create API documentation from source code comments in many programming languages (including C++, Python, PHP, etc.). This plugin automates the Doxygen process within your CI/CD workflow, providing immediate feedback on documentation coverage and build status directly within microCI.

A user should utilize this plugin whenever maintaining accurate, up-to-date developer guides or API references is a mandatory part of their deployment pipeline.

## ✨ Features
The `doxygen` plugin supports essential aspects of technical documentation generation:

*   **Document Source Code:** Automatically parses source files (`*.cpp`, `*.h`, etc.) to extract function signatures, class definitions, and internal logic descriptions placed within specialized comments (e.g., Javadoc-style or Qt-style).
    *   ***Use Case:*** Ensuring every public method and private helper function has clear docstrings explaining its purpose, parameters (`@param`), return values (`@return`), and potential exceptions (`@throws`).
*   **Document API:** Facilitates the creation of highly structured API documentation. By enforcing consistent commenting standards across different modules, developers can guarantee that external consumers of the library have a comprehensive reference guide available alongside the compiled artifact.
    *   ***Use Case:*** Generating a public-facing SDK reference manual that accurately reflects the current version's interface without manual intervention.

## ⚙️ Setup & Configuration
To enable and configure the Doxygen plugin, you must define the necessary steps in your CI/CD pipeline YAML file. The configuration requires specifying the Docker image and defining optional parameters for fine-grained control over the build process.

### Parameters Reference

| Parameter | Type | Default Value | Description | Required? |
| :--- | :--- | :--- | :--- | :--- |
| `docker` | String | N/A | The Docker image to use for running Doxygen (e.g., `intmain/microci_doxygen`). | Yes |
| `plugin.name` | String | `"doxygen"` | Specifies the plugin module being used. | Yes |
| `doxyfile` | Path | `./Doxyfile` | An alternative path to a custom Doxygen configuration file (`.doxyfile`). Overrides defaults. | No |
| `output_dir` | Path | `doxygen` | The directory where the generated documentation (HTML, man pages, etc.) will be placed. | No |
| `html` | Object | `{}` | Configuration for HTML generation output. | No |
| `header` | Path | N/A | Optional path to a custom header file (`@include`). | No |
| `footer` | Path | N/A | Optional path to a custom footer file. | No |
| `stylesheet` | Path | N/A | Optional CSS file to customize the appearance of the generated HTML documentation. | No |

### Example Configuration (`new/doxygen.yml`)

```yaml
steps:
  - name: "Build documentation with doxygen"
    description: "Build documentation with doxygen"
    docker: "intmain/microci_doxygen"
    plugin:
      name: doxygen
      #doxyfile: ./Doxyfile     # Default path for custom config
      #output_dir: doxygen      # Output directory for results
      html:
        #header: header.html    # Optional file to include in the head section
        #footer: footer.html    # Optional file to include in the footer section
        #stylesheet: style.css  # Optional path to a custom stylesheet
```

## 🚀 Examples
This example demonstrates how to integrate the Doxygen step into a pipeline, using a default configuration and specifying only the necessary Docker container and plugin name.

### Usage in CI/CD Pipeline YAML

Place this snippet within your main workflow file (e.g., `microci.yml`):

```yaml
# Example: Running Doxygen documentation build for the 'feature' branch
workflow:
  - name: "Documentation Generation"
    description: "Generates HTML API reference using doxygen plugin."
    steps:
      - name: "Build documentation with doxygen"
        docker: "intmain/microci_doxygen"
        plugin:
          name: doxygen
```

### Output Verification
After a successful run, the generated documentation artifacts will be available in the configured `output_dir` (defaulting to `doxygen`), typically containing an `index.html` file that can be published as static assets.