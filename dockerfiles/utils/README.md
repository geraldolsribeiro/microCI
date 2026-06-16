# microCI Utils image

This image provides a small utility runtime for **microCI** workflows, packaging helper tools used by pipeline steps that need a minimal but repeatable execution environment.

## How to use with microCI

Use this image from a pipeline step and run the required commands in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Run utility commands"
    docker: "intmain/microci_utils:0.11.0"
    plugin:
      name: bash
      bash: |
        uname -a
        env | sort
```

## Related documentation

- **microCI** docs: https://microci.dev
