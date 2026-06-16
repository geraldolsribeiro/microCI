# microCI GHDL image

This image supports VHDL workflows in **microCI**, packaging GHDL and related dependencies so VHDL analysis or simulation can run in a reproducible container.

## How to use with microCI

Use this image in a step that runs VHDL-related commands from `plugin.bash`.

Example:

```yaml
steps:
  - name: "Run VHDL checks"
    docker: "intmain/microci_ghdl:0.11.0"
    plugin:
      name: bash
      bash: |
        ghdl --version
```

## Related documentation

- **microCI** docs: https://microci.dev
