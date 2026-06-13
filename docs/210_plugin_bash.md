# ⚙️ Bash plugin

<center>![Plugin common](images/microci-plugin_common.svg)</center>

## What it does

Run custom shell commands inside a Docker container.

## Why it exists

The `bash` plugin keeps pipeline logic portable. It lets you execute the same shell step in a controlled container instead of tying it to a specific runner or host setup.

That means:

- reproducible execution
- isolated environments
- fewer host dependencies
- the same step on local machines, CI servers, and deployment systems

## When to use it

Use `bash` when a step needs shell commands, but you want the execution environment to stay explicit and reproducible.

## Example

```yaml
steps:
  - name: "Run shell commands"
    docker: "debian:stable-slim"
    plugin:
      name: bash
      bash: |
        echo 'hello from microCI'
```
