# 🛠️ The **clang-tidy** Plugin

## Overview

The **clang-tidy** plugin integrates the industrial-strength static analysis capabilities of Clang into your continuous integration workflow. It allows developers to enforce strict coding standards, identify potential bugs, and modernize C++ codebases automatically—all within the predictable structure of **microCI**. By catching issues like uninitialized variables, resource leaks, or violations of modern C++ idioms before compilation, this plugin significantly improves code quality, reduces runtime errors, and maintains consistency across large teams.

## Features

The `clang-tidy` plugin offers powerful control over static analysis processes:

### 🔍 Static Analysis & Linting
**Description:** This core feature analyzes the source code without executing it. It checks for potential bugs, adherence to coding guidelines (e.g., Google style guides), and structural inconsistencies using a massive suite of built-in checks.
**Use Case:** Mandatory integration in any CI pipeline step. Using `clang-tidy` ensures that every commit meets predefined quality gates, preventing low-quality or unsafe code from entering the mainline branch.

### ✨ Automatic Fix-its (Quick Fixes)
**Description:** Many detected issues are not merely warnings but suggestions for automated fixes. With this feature enabled (`fix: true`), **clang-tidy** can automatically apply simple corrections—such as updating variable declarations, adjusting style, or fixing modernizations—directly to the source files.
**Use Case:** Perfect for "cleanup" or "refactoring" CI steps. Instead of failing the build due to dozens of warnings, the pipeline fixes the issues and commits the changes, keeping the codebase clean with minimal human intervention.

### 🔧 Customizable Checks
**Description:** The plugin provides granular control over which specific checks run (e.g., `performance-*`, `modernize-*`). This allows teams to tailor the static analysis suite to match their project's unique requirements, focusing only on relevant warning categories without generating excessive noise from irrelevant checks.
**Use Case:** Used when a project is migrating standards or targeting niche performance improvements. By specifying precise check prefixes (e.g., `cppcoreguidelines-*`), teams can guide the code quality effort precisely where it is needed most.

## Setup & Configuration

### ⚙️ Installation

The `clang-tidy` plugin should be included in your `.microCI.yml` file within a step dedicated to code quality verification or refactoring. Ensure that the underlying Clang/LLVM toolchain necessary for deep static analysis is available in the execution environment's PATH.

### 📄 Configuration Parameters

Configuration is managed via nested parameters within the `plugin` block, allowing fine-grained control over the analysis scope and behavior.

| Parameter | Type | Description | Required? | Example Value |
| :--- | :--- | :--- | :--- | :--- |
| `checks` | Array<String> | A list of specific checks or check prefixes to enable/disable (e.g., `modernize-*`, `cppcoreguidelines-*`). Using `-*` enables all available checks. | No | `["-*", "performance-*"]` |
| `fix` | Boolean | If set to `true`, allows **clang-tidy** to automatically write fixes back to the source file. | No | `true` |
| `options` | Array<String> | Additional flags or compiler options (e.g., `-std=c++17`) passed to the underlying analysis tool. | No | `["-std=c++20"]` |
| `include` | Array<String> | Paths/directories that should be included during the compilation and analysis process. | Yes | `["include/"]` |
| `system_include` | Array<String> | System directories (like `/usr/local`) that must be available to the analyzer's include path. | No | `["/usr/local/include"]` |
| `source` | Array<String> | Explicit list of source files (`*.cpp`, `*.h`) that must be checked by this run. | Yes | `["src/*.cpp", "test/*.cpp"]` |

### Example Configuration (`.microCI.yml`)

To perform a comprehensive code quality check and automatically apply fixable warnings:

```yaml
steps:
    - name: "Code Quality Check & Auto-Fix with clang-tidy"
      plugin:
        name: "clang-tidy"
        checks:
          # Enforce all core guidelines, excluding specific areas if needed
          - "cppcoreguidelines-*", "-*!" 
        fix: true # Automatically applies fixes for warnings found
        options:
          - "-std=c++20" # Specify the required C++ standard version
        include:
          - "include/"
        system_include:
          - "/usr/local/include"
        source:
          - "src/*.cpp"
```

## Examples

This section demonstrates a complete example of how to utilize `clang-tidy` within the **microCI** pipeline, including defining multiple required parameters (sources, includes) for a robust analysis run.

```yaml
steps:
  - name: "Create C++ code verification report - clang-tidy"
    description: "Check C++ code and generate HTML report"
    network: bridge # Optional: Used for external resources/reports
    plugin:
      name: "clang-tidy"
      checks:
        - "-*"          # Run all available checks
        - "cppcoreguidelines-*" # Ensure compliance with core guidelines
      fix: true     # Automatically fix issues where possible
      options:
        - "-std=c++17"
      include:
        - "include/"
      system_include:
        - "/usr/local/include"
        - "/usr/include/"
      source:
        - "src/*.cpp"   # Check all CPP files in src/ directory
        - "test/*.cpp"  # And all test files
```