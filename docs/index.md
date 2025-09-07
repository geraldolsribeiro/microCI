# Welcome to microCI

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)

**microCI** is a powerful tool designed to streamline the creation of
continuous-integration pipelines by generating portable, auditable scripts for
executing tasks in Docker containers. This ensures that every step in the process
is fully reproducible across various environments:

* Developer's machines
* Servers for automatic builds after commits
* Servers for deployment to production

With microCI, the same tool, free of dependencies, can be used seamlessly in
both development and production environments. For complete documentation, visit
microci.dev.

The complete documentation can be found at [microci.dev](https://microci.dev).

## On the desktop

Using `microCI` on the developer's desktop is very simple:

```bash
microCI | bash
```

In the above command, `microCI` reads a `.microCI.yml` file containing the
description of the pipeline steps and generates a script for executing these
steps. This script is written to standard output and can be chained with other
tools or passed to a `bash` interpreter for execution.

A file called `.microCI.log` is generated with all messages, warnings, errors
from all executed commands and can be analyzed to resolve any problems that may
occur.

## On the server

To run on the server, it is sufficient to expose **webhooks** that, when
triggered, execute `microCI` in the respective project folders.

## Minimum requirements

* `bash`
* `docker`
* `jq`
* `yq`

## Docker Container

You can use **any Docker container** for execution, both public ones from
[Docker HUB](https://hub.docker.com/) and those you develop internally.

## Integration with github, gitlab, ...

The simplest and safest way to use `microCI` to read from or write to
repositories are to access them through an SSH key.

## Debian dependencies

For building standalone microCI binary the following dependencies should be installed:

```bash
sudo apt install gettext clang-format libspdlog-dev libfmt-dev libyaml-cpp-dev libssl-dev libcrypto++-dev
```

## Docker private repository

In some cases, running a private registry without SSL is necessary to add the
IP of the registry server to the `insecure-registries` section of the
`/etc/docker/daemon.json` file.

Example:

```bash
{
	"insecure-registries": ["11.22.33.44:5000", "192.168.1.1:5000"]
}
```

## Versions

* `0.38.0` Add option `--update-dev` to use development version
* `0.37.0` Execute Nth step; List steps; bugfixes
* `0.36.0` Allows to run devices inside the container without the `--privileged` flag
* `0.35.0` Plugin `vhdl-format`
* `0.34.0` Plugin `Doxygen` 
* `0.33.0` Global environment file parsing 
* `0.32.0` Integration config file generation
* `0.31.0` Plugin `template`
* `0.30.0` Plugin `pikchr`
* `0.29.0` Plugin `docmd`
* `0.28.0` Translation support added
* `0.27.0` Plugin `pandoc`
* `0.26.3` Update `clang-tidy`
* `0.26.0` Offline mode
* `0.25.6` Plugin `git_publish` with branch selection support (p.e. `gh-pages`)
* `0.24.0` Plugin `docker_build`
* `0.23.0` Plugin `fetch` download from GitHub (advanced)
* `0.22.0` Plugin `fetch` download from GitHub (folder)
* `0.21.0` Plugin `flawfinder`
* `0.20.0` Docker image `intmain/microci_clang`
* `0.19.0` Documentation with `intmain/docmd`
* `0.18.0` Plugin `minio`
* `0.17.0` Plugin `skip`; Execution status
* `0.16.1` Plugin refactoring; Default execution as non-root
* `0.15.2` Plugin `fetch` URL and gitlab folder
* `0.14.1` Plugin `beamer`
* `0.13.0` Docker network configuration
* `0.12.0` Plugins `clang-format` and `junit2html`
* `0.11.1` Discord channel notification
* `0.10.1` Multiple template added
* `0.9.0` Plugin `plantuml`
* `0.8.0` Plugin `clang-tidy`
* `0.7.0` Plugin `cppcheck`
* `0.6.1` Plugin `git_publish`
* `0.5.0` Volume mounting; SSH key copy
* `0.4.0` Building microCI by microCI
* `0.3.0` Configuration files creation
* `0.2.0` Plugin `mkdocs`
* `0.1.0` Plugin `bash` and `git_deploy`
* `0.0.1` Proof of concept/Initial version

