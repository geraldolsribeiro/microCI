# 🔍 flawfinder plugin

## What it does

Run security-focused source analysis.

## Why it exists

Use `flawfinder` when you want a portable security scan that runs as one step in the same pipeline definition.

## Example

```yaml
steps:
  - name: "Security scan"
    plugin:
      name: "flawfinder"
      sources: ["src"]
```
