# 🔍 doxygen plugin

## What it does

Generate API documentation from source comments.

## Why it exists

Use `doxygen` when documentation should be produced as part of the same portable pipeline that builds and checks the code.

## Example

```yaml
steps:
  - name: "Generate API documentation"
    plugin:
      name: "doxygen"
      config: "Doxyfile"
```
