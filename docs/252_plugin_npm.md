# 📦 npm plugin

## What it does

Run npm tasks inside a pipeline.

## Why it exists

Use `npm` when JavaScript builds, checks, and packaging steps should stay inside the same YAML-defined workflow.

## Example

```yaml
steps:
  - name: "Install dependencies"
    description: "Step description"
    bash: |
      npm install
  - name: "Check code"
    bash: |
      npm run lint --fix
  - name: "Build"
    bash: |
      npm run build
```
