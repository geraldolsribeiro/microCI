# Write your pipeline once. Execute it anywhere.

<div align="center">

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/geraldolsribeiro/microCI/total)

</div>

**microCI** is a portable pipeline engine: describe automation in YAML, generate plain Bash, and run the same pipeline locally, in CI, behind a webhook server, or in production.

For complete documentation, visit [microci.dev](https://microci.dev).

<div align="center">

![Portable Pipelines](docs/images/microci-ci_cd_pipeline.svg)

</div>

microCI is built around a simple idea:

- define the workflow once
- keep it auditable and reproducible
- execute it wherever you need

It fits many automation use cases, including:

- CI
- CD
- release automation
- documentation generation
- static site publishing
- container building
- firmware pipelines
- embedded software delivery
- internal operations

## How it works

**microCI** reads a YAML pipeline and generates a Bash script.

<div align="center">

![YAML to Bash](docs/images/microci-yaml_to_bash.svg)

</div>

The generated script can be executed directly, with no hidden runtime.

## Execute anywhere

<div align="center">

![How to execute](docs/images/microci-execute.svg)

</div>

Use the same pipeline definition in different environments:

```bash
microCI | bash
```

## Why microCI?

<div align="center">

![Where](docs/images/microci-where.svg)

</div>

microCI focuses on:

- pipeline portability
- auditability
- reproducibility
- vendor independence
- local-first execution

The result is one pipeline definition that can travel across environments without changing the source of truth.
