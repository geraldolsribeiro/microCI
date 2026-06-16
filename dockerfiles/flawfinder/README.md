# microCI Flawfinder image

This image runs Flawfinder security checks in **microCI**, packaging Flawfinder so C/C++ source auditing can run consistently inside a container.

## How to use with microCI

Use this image in a step that runs the `flawfinder` plugin.

Example:

```yaml
steps:
  - name: "Audit C/C++ sources"
    docker: "intmain/microci_flawfinder:0.11.0"
    plugin:
      name: flawfinder
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/244_plugin_flawfinder.md`

## Default plugin

This image is used by the `flawfinder` plugin.
