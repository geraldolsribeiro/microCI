# microCI Git image

This image provides Git tooling for **microCI** steps, packaging Git so pipelines can clone, inspect, tag, or publish repositories in a predictable environment.

## How to use with microCI

Use this image from a pipeline step and run the Git commands you need in `plugin.bash`.

Example:

```yaml
steps:
  - name: "Inspect repository"
    docker: "intmain/microci_git:0.11.0"
    plugin:
      name: bash
      bash: |
        git status
        git log --oneline -n 5
```

## Related documentation

- **microCI** docs: https://microci.dev
