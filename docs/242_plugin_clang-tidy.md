# 🔍 clang-tidy plugin

## What it does

Run static analysis on C++ code as part of a pipeline.

## Why it exists

Use `clang-tidy` when you want code quality checks to travel with the same pipeline definition across machines and environments.

## Example

```yaml
steps:
  - name: "Analyze C++ code"
    plugin:
      name: "clang-tidy"
      sources: ["src/*.cpp"]
      headers: ["include/**/*.hpp"]
```
