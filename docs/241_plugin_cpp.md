# 🧪 cpp plugin

## Overview

The `cpp` plugin runs C or C++ build commands from a **microCI** step. It is useful when you want a lightweight, explicit pipeline that compiles sources directly with GCC or Clang, or delegates to a build system such as `make`.

## Features

* **Direct compiler execution**  
  Run `gcc`, `g++`, `clang-22`, or `clang++-22` commands exactly as you would on a shell. This is ideal for small projects, quick checks, and reproducible build steps.

* **Build-system integration**  
  Use the same step to call `make` or other build tools when the project already has a build recipe. This keeps the pipeline simple while still supporting larger codebases.

* **Portable pipeline step**  
  The build logic lives in `.microCI.yml`, so the same step can be executed locally or in CI with the same behavior.

## Setup & Configuration

1. Create a step in `.microCI.yml`.
2. Set `plugin.name` to `cpp`.
3. Add the shell commands you want to run under `plugin.bash`.
4. Make sure the execution environment contains the compiler and tools you need (`gcc`, `g++`, `clang-22`, `clang++-22`, `make`, etc.).

### Configuration parameters

| Parameter | Type | Required | Description |
|---|---:|---:|---|
| `steps[]` | array | yes | Pipeline steps. |
| `steps[].name` | string | yes | Display name for the step. |
| `steps[].plugin.name` | string | yes | Must be `cpp`. |
| `steps[].plugin.bash` | string | yes | Shell script executed by the step. |
| `steps[].description` | string | no | Human-readable description of the step. |

## Example

```yaml
steps:
  - name: "Build static version of microCI"
    description: "Description of this step"
    plugin:
      name: cpp
      bash: |
        # Use GCC compiler directly
        gcc prog.c -o prog_c
        g++ prog.cpp -o prog_cpp
        # Test
        ./prog_c
        ./prog_cpp
        # Or build systems
        make clean
        # Use Clang compiler directly
        clang-22 prog.c -o prog_c
        clang++-22 prog.cpp -o prog_cpp
        ./prog_c
        ./prog_cpp
        make clean
```

## Notes

* Use `gcc` for C sources and `g++` for C++ sources.
* Use `clang-22` and `clang++-22` if you want to validate against Clang too.
* Keep the step focused: compile, test, or package in one clear action.
* For a full project build, combine `cpp` with your preferred build system inside `plugin.bash`.
