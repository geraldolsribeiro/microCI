# 📚 mkdocs_material plugin

## What it does

Build and serve documentation sites with MkDocs Material.

## Why it exists

Use `mkdocs_material` when documentation is part of the same portable pipeline as the code and release process.

## Example

```yaml
steps:
  - name: "Build documentation"
    plugin:
      name: "mkdocs_material"
      action: "build"
```
