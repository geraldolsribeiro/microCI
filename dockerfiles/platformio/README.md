# microCI PlatformIO image

This image is intended for building and flashing boards with [PlatformIO](https://platformio.org) inside microCI.
It bundles the PlatformIO toolchain and related dependencies so firmware builds, uploads, and board workflows can run in a reproducible container.

## How to use with microCI

Use this image in a pipeline step when you need to compile firmware or flash a connected board with PlatformIO commands.

Example:

```yaml
steps:
  - name: "Build and flash with PlatformIO"
    docker: "intmain/microci_platformio:latest"
    plugin:
      name: bash
      bash: |
        pio run
        pio run --target upload
```

## Related documentation

- **microCI** docs: https://microci.dev
- **PlatformIO** docs: https://docs.platformio.org
