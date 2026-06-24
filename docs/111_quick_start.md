# Quick start

The fastest way to understand microCI is to run short, meaningful examples.

## 1. Run a command

Create a file named `.microCI.yml` with this pipeline:

```yaml
steps:
  - name: "Run shell commands"
    docker: "debian:stable-slim"
    plugin:
      name: bash
      bash: |
        echo 'hello from microCI'
```

Run it with:

```bash
microCI | bash
```

## 2. Build a C++ hello world

Create another `.microCI.yml` with this pipeline:

```yaml
steps:
  - name: "Hello world step"
    docker: "gcc:14"
    plugin:
      name: bash
      bash: |
        g++ hello.cpp -o hello
```

Source file `hello.cpp`:

```cpp
#include <iostream>

int main() { std::cout << "Hello, World!\n"; }
```

Run it the same way:

```bash
microCI | bash
```

## What this shows

* write the pipeline in YAML format
* save it as `.microCI.yml`
* generate plain Bash
* run each step in an isolated Docker container using the image you specify
* keep the same idea across different tasks

That is the microCI model: one definition, one script, many uses.
