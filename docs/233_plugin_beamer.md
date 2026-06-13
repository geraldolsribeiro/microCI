# 📚 beamer plugin

## What it does

Generate PDF presentations from Markdown sources.

## Why it exists

Use `beamer` when slide generation should happen inside the same pipeline that produces the rest of the deliverables.

## Example

```yaml
steps:
  - name: "Build presentation"
    plugin:
      name: "beamer"
      input: "slides.md"
```
