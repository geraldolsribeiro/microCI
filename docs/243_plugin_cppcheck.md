# 🔍 cppcheck plugin

## What it does

Run static analysis on C++ sources.

## Why it exists

Use `cppcheck` to catch defects early while keeping the analysis step portable and repeatable.

## Example

```yaml
steps:
  - name: "Analyze C++ sources"
    plugin:
      name: "cppcheck"
      sources: ["src"]
      includes: ["include"]
```
