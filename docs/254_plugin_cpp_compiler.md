# 🖥️ C++ compiler image

## Overview

The **microCI** `cpp` compiler image provides a general-purpose C/C++ build and analysis environment.
Use it when your pipeline needs GCC, Clang, CMake, Ninja, cppcheck, Doxygen, Python tooling, and an ARM cross-toolchain for Linux targets.

## Features

* **C/C++ compilation**: compile sources with GCC or G++.
    * Use case: build native C and C++ projects inside a reproducible container.
* **Static analysis**: run Clang tools and cppcheck.
    * Use case: validate code quality and catch issues early in CI.
* **Documentation generation**: generate API docs with Doxygen.
    * Use case: publish reference documentation alongside your build artifacts.
* **Build system support**: use CMake or Make, with Ninja available when needed.
    * Use case: standardize project builds across developers and CI runners.
* **Cross-compilation tooling**: includes ARM GNU cross-compilers for Linux targets.
    * Use case: produce binaries for ARM-based systems from an x86_64 build host.

## Setup & Configuration

To use the image, configure a step in `.microCI.yml`:

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

* GCC and G++
* Clang 22, clang-format, clang-tidy
* cppcheck
* doxygen
* lcov
* cmake and ninja
* Python 3 and pip
* ARM GNU toolchain (`gcc-arm-linux-gnueabi`, `g++-arm-linux-gnueabi`)
