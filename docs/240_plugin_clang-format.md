# 🎨 clang-format plugin

## What it does

Format C++ source files as part of a pipeline.

## Why it exists

Use `clang-format` to keep style consistent while preserving the same portable pipeline definition across environments.

## Example

```yaml
steps:
  - name: "Format C++ sources"
    plugin:
      name: "clang-format"
      sources: ["src", "include", "test"]
```
