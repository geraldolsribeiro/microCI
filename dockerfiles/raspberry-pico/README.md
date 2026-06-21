# Raspberry Pi Pico build image

This image provides a ready-to-use build environment for Raspberry Pi Pico / RP2040 projects.
It includes the Pico SDK, CMake, the ARM cross toolchain, and `picotool` so microCI pipelines can build and inspect firmware images.

## What it is for

Use this image when a microCI step needs to:
- configure a Pico SDK project with CMake
- compile firmware for the RP2040
- generate `.uf2` / `.elf` artifacts
- use `picotool` for post-build inspection

## Example with microCI

```yaml
steps:
  - name: "Build Raspberry Pi Pico firmware"
    docker: "intmain/microci_raspberry_pico:latest"
    plugin:
      name: cpp
      bash: |
        mkdir -p build
        cd build
        cmake ..
        make
```

## Notes

- The Pico SDK is available in the container at `PICO_SDK_PATH=/opt/pico-sdk`
- `picotool` is installed and available via `picotool_DIR=/opt/picotool`
- See `dockerfiles/raspberry-pico/test_01/` for a minimal working example
