# Welcome to microCI

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)

**microCI** is a powerful tool designed to streamline the creation of
continuous integration pipelines by generating portable, auditable scripts for
executing tasks in Docker containers. This ensures that every step of the
process is fully reproducible across various environments:

* Developer's machines
* Servers for automatic builds after commits
* Servers for deployment to production

With microCI, the same tool, free of dependencies, can be used seamlessly on
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

* A properly configured Docker environment
* Bash shell
* `jq` and `yq` utilities

## Installation

The installation is equally simple, just copy the executable to a folder that
is in the `PATH`. To completely remove `microCI` from the system, just delete
the file copied to the PATH.

```bash
sudo curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/bin/microCI
sudo chmod 755 /usr/bin/microCI
```

## Docker Container

You can use **any Docker container** for execution, both public ones from
[Docker HUB](https://hub.docker.com/) and those you develop internally.

## Integration with github, gitlab, ...

The simplest and safest way to use `microCI` to read from or write to
repositories is to access them through an SSH key.

## To do

Below are some features that may be present in the next releases.

* create deb package
* create rpm package
* pass current user to container to avoid generating files as `root` in the working directory
* display error message from the failed step
* variable substitution in steps
* use of environment variables and secrets configured on the server
* create a Docker image from a git repository

## Debian dependencies

For building standalone microCI binary the following dependencies should be installed:

```bash
sudo apt install gettext clang-format libspdlog-dev libfmt-dev libyaml-cpp-dev libssl-dev libcrypto++-dev
```

## Docker private repository

In some cases, running a private registry without SSL, is necessary to add the
IP of registry server to the `insecure-registries` section of the
`/etc/docker/daemon.json` file.

Example:

```bash
{
	"insecure-registries": ["192.168.1.128:5000", "192.168.1.11:5000"]
}
```

## Versions

* `0.35.0` Plugin `vhdl-format` 
* `0.34.0` Plugin `doxygen` 
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
* `0.23.0` Plugin `fetch` download from github (advanced)
* `0.22.0` Plugin `fetch` download from github (folder)
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

## License

MIT License

Copyright (c) 2022-2024 Geraldo Luis da Silva Ribeiro

```
░░░░░░░░░░░░░░░░░
░░░░░░░█▀▀░▀█▀░░░
░░░█░█░█░░░░█░░░░
░░░█▀▀░▀▀▀░▀▀▀░░░
░░░▀░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░
```

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
