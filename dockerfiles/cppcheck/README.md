# microCI Cppcheck image

This image runs C/C++ static analysis in **microCI**, packaging `cppcheck` and its runtime dependencies so you can scan code in a reproducible container without preparing the host machine.

## How to use with microCI

Use this image in a step that runs the `cppcheck` plugin.

Example:

```yaml
steps:
  - name: "Analyze C++ sources"
    docker: "intmain/microci_cppcheck:0.11.0"
    plugin:
      name: cppcheck
      sources: ["src"]
      includes: ["include"]
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/243_plugin_cppcheck.md`

## Default plugin

This image is used by the `cppcheck` plugin.
