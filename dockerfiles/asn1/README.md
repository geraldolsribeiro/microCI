# microCI ASN.1 image

This image exists to provide the tools needed by **microCI** steps that work with ASN.1 artifacts.

## How to use with microCI

Reference this image in a pipeline step and run the commands you need inside `plugin.bash`.

Example:

```yaml
steps:
  - name: "Process ASN.1 artifacts"
    docker: "intmain/microci_asn1:0.11.0"
    plugin:
      name: bash
      bash: |
        your-asn1-command --help
```

## Related documentation

- **microCI** docs: https://microci.dev
