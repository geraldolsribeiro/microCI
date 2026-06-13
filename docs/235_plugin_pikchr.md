# 📚 pikchr plugin

## What it does

Render diagrams from Pikchr source.

## Why it exists

Use `pikchr` when diagrams should be generated as part of the same reproducible pipeline as the documentation.

## Example

```yaml
steps:
  - name: "Render diagram"
    plugin:
      name: "pikchr"
      input: "diagram.pikchr"
```
