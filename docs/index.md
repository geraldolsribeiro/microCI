# Write your pipeline once. Execute it anywhere.

<div align="center" markdown="1">

[![C/C++ CI](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/geraldolsribeiro/microCI/actions/workflows/c-cpp.yml)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/geraldolsribeiro/microCI/total)
![GitHub Release Date](https://img.shields.io/github/release-date/geraldolsribeiro/microCI)
![GitHub Commit Activity](https://img.shields.io/github/commit-activity/t/geraldolsribeiro/microCI)
![GitHub Created At](https://img.shields.io/github/created-at/geraldolsribeiro/microCI)

</div>

## Why microCI exists

Traditional CI systems lock your workflow to a vendor:

* GitHub Actions -> GitHub
* GitLab CI -> GitLab
* Jenkins -> Jenkins

microCI breaks that lock-in.

Define automation in YAML. Generate plain Bash. Run the same pipeline anywhere.

* locally
* on CI servers
* on deployment servers
* in containers
* behind webhooks

<div align="center" markdown="1">

![Portable Pipelines](images/microci-ci_cd_pipeline.svg)

</div>

## What microCI does

### Plugins

* [bash](210_plugin_bash.md)
* [skip](211_plugin_skip.md)
* [fetch](212_plugin_fetch.md)
* [git_deploy](220_plugin_git_deploy.md)
* [git_publish](221_plugin_git_publish.md)
* [mkdocs_material](230_plugin_mkdocs_material.md)
* [pandoc](232_plugin_pandoc.md)
* [beamer](233_plugin_beamer.md)
* [plantuml](234_plugin_plantuml.md)
* [pikchr](235_plugin_pikchr.md)
* [docmd](236_plugin_docmd.md)
* [doxygen](237_plugin_doxygen.md)
* [asciidoc](238_plugin_asciidoc.md)
* [clang-format](240_plugin_clang-format.md)
* [cpp](241_plugin_cpp.md)
* [clang-tidy](242_plugin_clang-tidy.md)
* [cppcheck](243_plugin_cppcheck.md)
* [flawfinder](244_plugin_flawfinder.md)
* [jfrog](250_plugin_jfrog.md)
* [minio](251_plugin_minio.md)
* [npm](252_plugin_npm.md)
* [raspberry_pico](253_plugin_raspberry_pico.md)
* [cpp compiler image](254_plugin_cpp_compiler.md)


microCI is built around a simple idea:

* write it once
* review it easily
* reproduce it anywhere

It fits many automation use cases:

* CI and CD
* release automation
* documentation generation
* static site publishing
* container building
* firmware delivery
* embedded software delivery
* Raspberry Pi Pico firmware builds
* internal operations

**microCI** reads a YAML pipeline and generates a Bash script.

For Raspberry Pi Pico projects, see [Raspberry Pico plugin](253_plugin_raspberry_pico.md).

<div align="center" markdown="1">

**YAML to Bash**

![YAML to Bash](images/microci-yaml_to_bash.svg)

</div>

The generated script can be executed directly. No hidden platform. No vendor lock-in.

## Execute anywhere

Use the same pipeline definition in different environments:

```bash
microCI | bash
```

<div align="center" markdown="1">

![How to execute](images/microci-execute.svg)

</div>

## Why it matters

microCI focuses on:

* pipeline portability
* auditability
* reproducibility
* vendor independence
* local-first execution

The result: one pipeline definition, one source of truth, one script, everywhere.
