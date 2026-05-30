# clang-format plugin

The **microCI** `clang-format` plugin formats C++ source files in your pipeline using `clang-format`. It is useful for enforcing a consistent code style across source, test, and header files before changes are merged.

## Features

- **Apply formatting to source code**  
  Runs `clang-format` over the configured file set to standardize formatting. Use this to keep C++ code style consistent across the repository.

## Setup & Configuration

Add the plugin to a step in your `.microCI.yml` pipeline.

### Configuration parameters

| Parameter | Required | Description | Example |
|---|---:|---|---|
| `name` | Yes | Plugin identifier. | `clang-format` |
| `source` | Yes | List of file paths or glob patterns to format. | `src/*.cpp` |

### Example configuration

```yaml
steps:
  - name: "Format C++ code with clang-format"
    description: "Standardizes code formatting using rules"
    run_as: user
    plugin:
      name: "clang-format"
      source:
        - "src/*.cpp"
        - "test/*.cpp"
        - "include/*.hpp"
```

## Example

A complete `.microCI.yml` step using the plugin:

```yaml
steps:
  - name: "Format C++ code with clang-format"
    description: "Standardizes code formatting using rules"
    run_as: user
    plugin:
      name: "clang-format"
      source:
        - "src/*.cpp"
        - "test/*.cpp"
        - "include/*.hpp"
```

## Notes

- The `source` field accepts file patterns, so you can target specific directories or extensions.
- Use this plugin as part of a formatting or quality-control stage in **microCI**.
