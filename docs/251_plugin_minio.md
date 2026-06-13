# 📦 minio plugin

## What it does

Interact with S3-compatible object storage from a pipeline.

## Why it exists

Use `minio` when build artifacts, logs, or generated files need to be stored outside the repository in a reproducible way.

## Example

```yaml
steps:
  - name: "Upload artifacts"
    plugin:
      name: "minio"
      action: "cp"
      source: "site"
      target: "s3://bucket/site"
```
