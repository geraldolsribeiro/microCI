# microCI PlantUML image

This image renders PlantUML diagrams in **microCI**, packaging PlantUML and its runtime dependencies so diagrams can be rendered in a reproducible container.

## How to use with microCI

Use this image in a step that runs the `plantuml` plugin.

Example:

```yaml
steps:
  - name: "Render PlantUML diagrams"
    docker: "intmain/microci_plantuml:0.11.0"
    plugin:
      name: plantuml
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/234_plugin_plantuml.md`

## Default plugin

This image is used by the `plantuml` plugin.
