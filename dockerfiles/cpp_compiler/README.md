# microCI C++ compiler image

This image provides a ready-to-use C/C++ toolchain for **microCI** pipelines.

## How to use with microCI

Use this image in a step that runs the `cpp` plugin, or any other plugin that needs a compiler toolchain.

Example:

```yaml
steps:
  - name: "Build C++ project"
    docker: "intmain/microci_cpp_compiler:0.11.0"
    plugin:
      name: cpp
      bash: |
        g++ prog.cpp -o prog_cpp
        gcc prog.c -o prog_c
```

## Related documentation

- **microCI** docs: https://microci.dev
- Plugin documentation: `docs/241_plugin_cpp.md`

## Default plugin

This image is used by the `cpp` plugin.
