# microCI OMNeT++ image

This image supports OMNeT++ workflows in **microCI**, packaging the dependencies required to build or run OMNeT++-based projects in a reproducible container.

## How to use with microCI

Use this image from a pipeline step and run OMNeT++ commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Build OMNeT++ project"
    docker: "intmain/microci_omnetpp:0.11.0"
    plugin:
      name: bash
      bash: |
        make
```

## Related documentation

- **microCI** docs: https://microci.dev
