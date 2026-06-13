# 📚 pandoc plugin

## What it does

Convert documents into other formats during a pipeline run.

## Why it exists

Use `pandoc` when documentation, release notes, or publishing outputs need to be generated as part of the same portable workflow.

## Example

```yaml
steps:
  - name: "Convert markdown"
    plugin:
      name: "pandoc"
      input: "README.md"
      output: "README.pdf"
```
