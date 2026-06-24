# 🖥️ C++ compiler image

This image provides a ready-to-use C/C++ build and analysis environment for **microCI** pipelines.
It includes GCC, Clang, CMake, Ninja, `cppcheck`, `doxygen`, Python tooling, and an ARM cross-toolchain for Linux targets.

## Overview

Use this image when a **microCI** step needs to:
- compile C or C++ sources
- run static analysis with Clang or cppcheck
- generate documentation with Doxygen
- build projects with CMake or Make
- cross-compile for ARM Linux targets

## Setup & Configuration

Configure the step in `.microCI.yml`:

| Parameter | Required | Description |
|---|---:|---|
| `steps[].name` | yes | Human-readable step name shown in logs |
| `steps[].docker` | yes | Docker image that provides the compiler environment |
| `steps[].plugin.name` | yes | Usually `cpp` |
| `steps[].plugin.bash` | yes | Shell commands executed inside the container |

## Example

```yaml
steps:
  - name: "Build C++ sources"
    docker: "intmain/microci_cpp_compiler:latest"
    plugin:
      name: cpp
      bash: |
        rm -rf build
        cmake -S . -B build
        cmake --build build
```

## Included tools

- GCC and G++
- Clang 22, clang-format, clang-tidy
- cppcheck
- doxygen
- lcov
- cmake and ninja
- Python 3 and pip
- ARM GNU toolchain (`gcc-arm-linux-gnueabi`, `g++-arm-linux-gnueabi`)

## Notes

- This image is intended for general C/C++ build and analysis workflows.
- The default command only prints an informational message; it is meant to be used through **microCI**.
