# Welcome to microCI

The complete documentation can be found at [microci.dev](https://microci.dev).

`microCI` is a tool to assist in building continuous integration pipelines.

The main differentiator of `microCI` is the generation of **portable scripts**
and fully **auditable** for executing tasks in **docker containers**.

Since all steps are executed in one or more docker containers, the entire
process becomes reproducible on: a) any developer's machine, b) the server that
will perform automatic builds after commits, c) the server that will perform
deployment to production... In short, the same tool, without dependencies, can
be used on both the developer's machine and the server.

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

## Installation

The installation is equally simple, just copy the executable to a folder that
is in the `PATH`. To completely remove `microCI` from the system, just delete
the file copied to the PATH.

```bash
curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/bin/microCI
chmod 755 /usr/bin/microCI
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

## Versions

* `0.30.0` Plugin `pikchr`
* `0.29.0` Plugin `docmd`
* `0.28.0` Translation support added
* `0.27.0` Plugin `pandoc`
* `0.26.3` Update `clang-tidy`
* `0.26.0` Offline mode
* `0.25.6` Plugin `git_publish` com escolha de branch (p.e. `gh-pages`)
* `0.24.0` Plugin `docker_build`
* `0.23.0` Plugin `fetch` download github
* `0.22.0` Plugin `fetch` download de pastas no github
* `0.21.0` Plugin `flawfinder`
* `0.20.0` Imagem `intmain/microci_clang`
* `0.19.0` Documentação com `intmain_docmd`
* `0.18.0` Plugin `minio`
* `0.17.0` Plugin `skip`; Status de execução
* `0.16.1` Refatoração dos plugins; Execução default como não root
* `0.15.2` Plugin `fetch` URL e pastas do gitlab
* `0.14.1` Plugin `beamer`
* `0.13.0` Configuração de rede docker
* `0.12.0` Plugins `clang-format` e `junit2html`
* `0.11.1` Notificação em canal do discord
* `0.10.1` Múltiplos arquivos de modelo
* `0.9.0` Plugin `plantuml`
* `0.8.0` Plugin `clang-tidy`
* `0.7.0` Plugin `cppcheck`
* `0.6.1` Plugin `git_publish`
* `0.5.0` Montagem de volumes; Cópia de chave SSH
* `0.4.0` Construção do microCI pelo microCI
* `0.3.0` Cria arquivos de configuração
* `0.2.0` Plugin `mkdocs`
* `0.1.0` Passo em `bash` e plugin `git_deploy`
* `0.0.1` Versão inicial para prova de conceito

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
 
