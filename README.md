# Welcome to microCI

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)

**microCI** is a powerful tool designed to streamline the creation of
continuous-integration pipelines by generating portable, auditable scripts for
executing tasks in Docker containers.

For complete documentation, visit [microci.dev](https://microci.dev).

<center>![CI/CD Pipeline](docs/images/microci-ci_cd_pipeline.svg)</center>

CI/CD stands for Continuous Integration / Continuous Deployment. CI and CD
pipelines form the backbone of modern software development, enabling teams to
ship features faster and more reliably. A solid CI/CD pipeline architecture is
key to making these workflows scalable, secure, and efficient—not just
automated.


## How it works?

**microCI** creates a Bash script that implements the pipeline defined in a YAML file.

<center>![YAML to Bash](docs/images/microci-yaml_to_bash.svg)</center>

Each step in the pipeline recipe is executed by its own portable Bash function.


## How to execute?

<center>![How to execute](docs/images/microci-execute.svg)</center>

Using `microCI` is very simple:

```bash
microCI | bash
```

## Where can I use microCI?

<center>![Where](docs/images/microci-where.svg)</center>

Every step in the process is fully reproducible across various environments:

* Developer workstations
* Build servers triggered by commits
* Deployment servers for production releases

With microCI, the same zero-dependency tool works seamlessly in both
development and production environments.

