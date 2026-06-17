# Write your pipeline once. Execute it anywhere.

<div align="center" markdown="1">

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/geraldolsribeiro/microCI/total)

</div>

## Why microCI exists

Traditional CI systems lock your workflow to a vendor:

- GitHub Actions -> GitHub
- GitLab CI -> GitLab
- Jenkins -> Jenkins

microCI breaks that lock-in.

Define automation in YAML. Generate plain Bash. Run the same pipeline anywhere.

- locally
- on CI servers
- on deployment servers
- in containers
- behind webhooks

<div align="center" markdown="1">

![Portable Pipelines](docs/images/microci-ci_cd_pipeline.svg)

</div>

## What microCI does

microCI is built around a simple idea:

- write it once
- review it easily
- reproduce it anywhere

It fits many automation use cases:

- CI and CD
- release automation
- documentation generation
- static site publishing
- container building
- firmware delivery
- embedded software delivery
- internal operations

**microCI** reads a YAML pipeline and generates a Bash script.

<div align="center" markdown="1">

**YAML to Bash**

![YAML to Bash](docs/images/microci-yaml_to_bash.svg)

</div>

The generated script can be executed directly. No hidden platform. No vendor lock-in.

## Execute anywhere

Use the same pipeline definition in different environments:

```bash
microCI | bash
```

<div align="center" markdown="1">

![How to execute](docs/images/microci-execute.svg)

</div>

## Why it matters

microCI focuses on:

- pipeline portability
- auditability
- reproducibility
- vendor independence
- local-first execution

The result: one pipeline definition, one source of truth, one script, everywhere.
