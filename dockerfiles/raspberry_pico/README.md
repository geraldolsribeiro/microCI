# 📟 Raspberry Pico plugin

This image provides a ready-to-use build environment for Raspberry Pi Pico family projects based on RP2040 and RP2350.
It includes the Pico SDK, CMake, the ARM cross-toolchain, and `picotool` so **microCI** pipelines can build and inspect firmware images for `pico`, `pico_w`, `pico2`, and `pico2_w`.

## Overview

Use this image when a **microCI** step needs to:
- configure a Pico SDK project with CMake
- compile firmware for the RP2040
- generate `.uf2` / `.elf` artifacts
- use `picotool` for post-build inspection

## Setup & Configuration

Configure the step in `.microCI.yml`:

| Parameter | Required | Description |
|---|---:|---|
| `steps[].name` | yes | Human-readable step name shown in logs (for example, `Build Raspberry Pi Pico firmware`) |
| `steps[].docker` | yes | `intmain/microci_raspberry_pico:latest` |
| `steps[].plugin.name` | yes | Must be `raspberry_pico` |
| `steps[].plugin.board` | no | Target board variant (`pico`, `pico_w`, `pico2`, `pico2_w`) |
| `steps[].plugin.bash` | yes | Shell commands executed inside the container |

## Environment

| Variable | Value |
|---|---|
| `PICO_SDK_PATH` | `/opt/pico-sdk` |
| `picotool_DIR` | `/opt/picotool` |

## Example

```yaml
steps:
  - name: "Build Raspberry Pi Pico firmware"
    docker: "intmain/microci_raspberry_pico:latest"
    plugin:
      name: raspberry_pico
      board: pico2 # pico | pico_w | pico2 | pico2_w
      bash: |
        rm -rf build
        cmake -S . -B build
        cmake --build build
```

## Notes

- The Pico SDK is included in the container.
- `picotool` is installed and ready for use.
- The image is intended for Raspberry Pi Pico family firmware projects based on RP2040 and RP2350 (`pico`, `pico_w`, `pico2`, `pico2_w`).
