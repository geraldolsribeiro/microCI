# 📟 Raspberry Pico plugin

## Overview

The **microCI** `raspberry_pico` plugin provides a Raspberry Pi Pico build environment for RP2040 and RP2350 devices.
Use it when your pipeline needs the Pico SDK, CMake, the ARM cross-toolchain, and `picotool` to build firmware images for `pico`, `pico_w`, `pico2`, or `pico2_w`.

## Features

* **Pico SDK environment**: ships the Raspberry Pi Pico SDK for building Pico family projects.
    * Use case: compile firmware without installing the SDK on the host.
* **Cross-compilation toolchain**: includes the ARM toolchain required for Pico targets.
    * Use case: build `.elf` and `.uf2` firmware artifacts.
* **CMake-based builds**: supports standard Pico SDK CMake workflows.
    * Use case: configure and build projects using `cmake -S . -B build`.
* **picotool support**: provides `picotool` for post-build inspection and firmware handling.
    * Use case: inspect generated binaries or prepare images for deployment.

## Setup & Configuration

To use the plugin, configure a step in `.microCI.yml` with:
* `docker: "intmain/microci_raspberry_pico:latest"`
* `plugin.name: "raspberry_pico"`
* `plugin.board`: target board selector (`pico`, `pico_w`, `pico2`, `pico2_w`)
* `plugin.bash`: shell commands executed inside the container

### Configuration reference

| Parameter | Required | Description |
|---|---:|---|
| `steps[].name` | yes | Human-readable step name shown in logs (for example, `Build Raspberry Pi Pico firmware`) |
| `steps[].docker` | yes | Docker image that provides the Pico build environment |
| `steps[].plugin.name` | yes | Must be `raspberry_pico` |
| `steps[].plugin.board` | no | Target board variant (`pico`, `pico_w`, `pico2`, `pico2_w`) |
| `steps[].plugin.bash` | yes | Shell script run by the step |

### Environment variables

| Variable | Value | Purpose |
|---|---|---|
| `PICO_SDK_PATH` | `/opt/pico-sdk` | Location of the Pico SDK |
| `picotool_DIR` | `/opt/picotool` | Location of `picotool` |

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

* The image is intended for Raspberry Pi Pico family firmware projects based on RP2040 and RP2350 (`pico`, `pico_w`, `pico2`, `pico2_w`).
* The same documentation is mirrored in `dockerfiles/raspberry_pico/README.md` for image-specific usage.
