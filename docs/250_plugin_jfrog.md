# 📦 jfrog plugin

## What it does

Interact with artifact repositories from a pipeline.

## Why it exists

Use `jfrog` when a pipeline needs to upload, download, or manage artifacts without leaving the same YAML-defined workflow.

## Example

```yaml
steps:
  - name: "Publish artifacts"
    plugin:
      name: "jfrog"
      action: "upload"
```
