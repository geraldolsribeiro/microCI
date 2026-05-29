# cppcheck Plugin Documentation (cppcheck)

## 🚀 Overview
The **cppcheck** plugin integrates powerful static analysis into the **microCI** pipeline, allowing developers to enforce high levels of code quality, memory safety, and security compliance directly within their CI/CD workflow. Instead of relying on runtime tests, this plugin analyzes C++ source code at compile time, identifying potential bugs, anti-patterns, and security vulnerabilities before the code ever reaches production or even local testing environments.

By making static analysis a mandatory step in **microCI**, teams can establish an automated quality gate that significantly improves overall code robustness and maintainability.

## ✨ Features
### Deep Memory Safety
**Description:** cppcheck rigorously detects common memory management errors, such as use-after-free, double deletion, and buffer overflows.
**Use Case:** Essential for mission-critical systems or any codebase handling complex resource lifetimes where traditional testing might miss subtle memory leaks or corruption issues.

### Bi-directional Analysis
**Description:** Unlike tools that only check forward dependencies (A calls B), cppcheck can analyze data flow both into and out of functions, providing a much more complete understanding of variable usage and potential state changes throughout the program.
**Use Case:** Identifying subtle bugs where an input parameter is modified unexpectedly or where side effects occur across function boundaries.

### Security & Compliance Standards
**Description:** The plugin helps enforce coding standards that align with industry best practices and security guidelines (e.g., detecting insecure usage of standard library functions, potential SQL injection vectors, etc.).
**Use Case:** Ensuring adherence to internal compliance policies or preparing code for environments requiring high levels of security auditing (e.g., financial, medical).

### Extensive Language Support (C++11, C++14, C++17, and partial support for C++20)
**Description:** Provides comprehensive checking across multiple modern C++ standards, allowing development teams to gradually upgrade their codebase while maintaining quality assurance.
**Use Case:** Facilitating large-scale code modernization efforts by validating syntax and patterns specific to newer language features.

### Environment Agnostic
**Description:** The plugin's core logic is decoupled from the immediate execution environment details (like OS or specific build tool versions), making it highly portable across different CI runners.
**Use Case:** Supporting development teams that use a diverse set of build environments or need consistency regardless of underlying infrastructure changes.

### Seamless Integration
**Description:** Designed specifically for **microCI**, integration is achieved via declarative configuration in the `.microCI.yml` file, making it as simple to enable as any other plugin step.
**Use Case:** Rapid setup and maintenance of CI/CD pipelines without needing deep knowledge of underlying static analysis tooling command lines.

## ⚙️ Setup & Configuration
To utilize the `cppcheck` plugin, you must define its execution details within a step in your `.microCI.yml`. The configuration allows fine-tuning of the analysis scope, language standard, and specific checks to be performed.

### Parameters

| Parameter | Type | Description | Required? | Notes |
| :--- | :--- | :--- | :--- | :--- |
| `name` | String | A descriptive name for the CI step (e.g., "cppcheck Code Analysis"). | Yes | Used for reporting purposes within **microCI**. |
| `plugin.name` | String | The identifier used to invoke this plugin. | Yes | Must be set to `"cppcheck"`. |
| `include` | List[String] | A list of directories or files containing source code that must be analyzed. | Yes | All relevant source paths should be listed here. |
| `source` | List[String] | Alternative listing for source folders/files, potentially used for multiple analysis passes. | No | Generally use `include` unless specific staging is required. |
| `platform` | String | The target platform or architecture for which the code is being compiled (e.g., `unix64`, `win32`). | Recommended | Helps narrow down environment-specific checks. |
| **Options** | Dictionary/List[String] | Specific command-line options to pass directly to cppcheck. | Optional | Includes parameters like `--std=c++17` or specific job limits (`-j 4`). |

## 💻 Examples
The following example demonstrates how to use the **cppcheck** plugin within a standard **microCI** workflow, performing a comprehensive code check on source and test directories while enforcing C++ standards.

### Configuration File: `.microCI.yml` (Excerpt)

```yaml
steps:
  - name: "C++ Code Verification"
    description: "Check C++ code and generate HTML report for static analysis."
    plugin:
      name: "cppcheck"
      options:
        # Use clang as the analyzer, enforcing C++17 standard
        - "--clang=clang-13" 
        - "--std=c++17"       
        - "--bug-hunting"     
      include:
        - "include"           # Analyze public header files
        - "src"               # Analyze source implementation files
        - "test"              # Analyze test cases for correctness issues
      platform: "unix64" # Target environment is Unix 64-bit
```

### Summary of Analysis Scope
*   **Target Codebase:** The plugin analyzes code in `include/`, `src/`, and `test/`.
*   **Standards enforced:** C++17 (via `options`).
*   **Output:** A detailed report summarizing all detected issues, categorized by severity.

***
*This documentation was generated by the Documentation Generator Skill.*