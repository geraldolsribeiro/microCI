# 📚 plantuml plugin

## What it does

Render PlantUML diagrams from text sources.

## Why it exists

Use `plantuml` when architecture diagrams belong in the same portable workflow as the rest of the documentation.

## Example

```yaml
steps:
  - name: "Render PlantUML"
    plugin:
      name: "plantuml"
      input: "diagram.puml"
```
