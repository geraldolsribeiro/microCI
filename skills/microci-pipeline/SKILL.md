---
name: microci-pipeline
description: Create and edit microCI pipeline files (.microCI.yml) from plain-language requests. Covers the microCI YAML schema, all plugins (bash, unsafe, fetch, git_publish, git_deploy, mkdocs_material, pandoc, docmd, doxygen, asciidoc, beamer, plantuml, pikchr, mermaid, cpp, clang-format, clang-tidy, cppcheck, flawfinder, vhdl-format, minio, jfrog, npm, docker_build, raspberry_pico, skip), environment files, and step execution. Documentation is NOT bundled; it is fetched/resynced on demand from the upstream GitHub repo so it always matches the latest binary. Use for any task involving .microCI.yml, microCI plugins, microCI steps, or "add/edit a CI step" requests in a microCI project.
---

# microCI Pipeline Skill

Skill for reading, creating and editing **microCI** pipeline files (`.microCI.yml`) from plain-language
requests. microCI reads a YAML pipeline and generates a Bash script (`microCI | bash`).

This SKILL.md is only a router. The authoritative schema lives in the **synced cache**
(plugin docs + canonical templates), fetched from GitHub on demand. See
`references/bootstrap.md` for the sync procedure and `references/common-fields.md` for the
small stable core (file structure, step-level fields, validation commands).

## Critical Rules (anti-guessing)

1. **Never invent a YAML key.** A plugin field is valid only if it appears in the synced cache:
   `docs/<NNN>_plugin_<name>.md` or `templates/<plugin>.yml`. If neither documents what the user
   asked for, STOP and ask the user — do not guess names, defaults, or behavior.
2. **Sync before first edit of a session** (or when the cache is missing/stale). An offline agent
   with no usable cache must tell the user it cannot safely edit the file, not improvise.
3. **Edit in place.** Preserve existing comments, step order, blank lines and quoting style.
   Never regenerate/reformat a whole `.microCI.yml` that already exists — apply minimal targeted
   edits (the community files are heavily commented; comments are user intent).
4. **Validate after every edit** (see Validation below): `microCI -T -i .microCI.yml` then
   `microCI -l -i .microCI.yml`. The job is not done until both pass.
5. **One file:** the pipeline file is `.microCI.yml` in the project root; other files are used via
   `-i <file>`. Do not invent derivative filenames.
6. **Plugin name strings matter:** they are exactly as in the templates, e.g. `clang-format`,
   `clang-tidy`, `git_publish`, `mkdocs_material`, `raspberry_pico` (hyphens/underscores differ).
7. **Secrets go through environment files**, never hardcoded: `.env` (`VAR=VALUE`),
   `.env.yml` (`VAR: VALUE`) or `$HOME/.microCI.env`. Reference them in steps as `"${VAR}"`.
8. The `unsafe` plugin runs on the host and **rejects** these step fields:
   `docker`, `network`, `volumes`, `run_as`, `devices`, `ssh`.


## How to Use This Skill

### Cache layout (created by the sync, never hand-edited)

| Path | Content | When to Read |
|------|---------|--------------|
| `<cache>/docs/<NNN>_plugin_<name>.md` | Plugin overview + parameter tables + examples | Before configuring/using a plugin |
| `<cache>/docs/1xx_*.md` | Quick start, env vars, CLI help, GUI/DISPLAY | Concepts, CLI options |
| `<cache>/templates/<plugin>.yml` | Canonical commented skeleton (= `microCI --new <plugin>` output) | When adding a new step — start from this |
| `<cache>/.sync_meta` | Source repo, commit SHA, sync timestamp | To judge freshness |

Cache location: `%LOCALAPPDATA%\microci-skill` (Windows) or `~/.cache/microci-skill` (Linux/macOS).

### Sync / resync triggers

- First use in a session and `<cache>/.sync_meta` is missing, malformed, or older than 24 h.
- The user asks to "resync/refresh microCI docs".
- A key the user references is not found in the cached docs (maybe it is new upstream).

Three equivalent ways (details in `references/bootstrap.md`):
1. Run `scripts/sync.ps1` (Windows PowerShell) or `scripts/sync.sh` (Linux/macOS bash/curl).
2. Web-fetch only: GitHub contents API for listings + raw.githubusercontent.com for files
   (works with any agent that has an HTTP fetch tool; no shell needed).
3. `git clone --depth 1 https://github.com/geraldolsribeiro/microCI` and copy `docs/*.md` + `new/*.yml`.

### Decision tree: which plugin does the user want?

```
What should the step do?
+-- Run shell commands (in a container)        -> bash          (docs 210, template bash.yml)
+-- Run commands directly on the host          -> unsafe        (docs 299, template unsafe.yml)
+-- Download external files/deps               -> fetch         (docs 212, template fetch.yml)
+-- Publish site/branch to a git repo          -> git_publish   (docs 221, template git_publish.yml)
+-- Deploy to production server via git        -> git_deploy    (docs 220, template git_deploy.yml)
+-- Build docs with mkdocs                     -> mkdocs_material (docs 230, template mkdocs_material.yml)
+-- Configure mkdocs theme from README         -> mkdocs_material_config (docs 231)
+-- Convert documents (md/odt/pdf...)          -> pandoc        (docs 232, template pandoc.yml)
+-- PDF presentation                           -> beamer        (docs 233, template beamer.yml)
+-- Diagrams                                   -> plantuml (234) | pikchr (235) | mermaid (template mermaid.yml)
+-- Extract inline docs                        -> docmd (236)   | doxygen (237)
+-- AsciiDoc processing                        -> asciidoc      (docs 238, template asciidoc.yml)
+-- Format C++ code                            -> clang-format  (docs 240, template clang-format.yml)
+-- Build C++ with makefile/cmake              -> cpp           (docs 241, template cpp.yml)
+-- C++ static analysis                        -> clang-tidy (242) | cppcheck (243) | flawfinder (244)
+-- Format VHDL                                -> vhdl-format   (template vhdl-format.yml)
+-- Artifact upload/download                   -> minio (251) | jfrog (250) | npm (252)
+-- Raspberry Pi Pico firmware                 -> raspberry_pico (docs 253, template raspberry_pico.yml)
+-- Build a docker image locally               -> docker_build  (template docker_build.yml)
+-- Disable a step temporarily                 -> skip          (docs 211, template skip.yml)
```

If the intent is unclear between plugins, ask the user before writing anything.

### Decision tree: what does the task need?

```
Task?
+-- Add a new step            -> sync cache; copy <cache>/templates/<plugin>.yml as base;
|                                strip comment guidance not needed; append under `steps:`; validate
+-- Edit an existing step     -> read file; locate step by name/description; change ONLY the keys
|                                the user asked about (keep comments/order); validate
+-- Disable a step            -> set plugin.name to skip (docs 211) — or ask if deletion is wanted
+-- Change container image    -> step key `docker: "image:tag"` (default debian:stable-slim;
|                                global default via top-level `docker:`)
+-- Network access needed     -> step key `network: bridge` (options: none [default] | bridge | host)
+-- Env vars / secrets        -> step `envs:` map (values may use "${VAR}"); define VAR in .env,
|                                .env.yml or $HOME/.microCI.env (docs 113); global `envs:` at top level
+-- Mount a folder/device     -> step `volumes:` [{source, destination, mode}] / `devices:` list
+-- SSH keys into container   -> step `ssh: {copy_from, copy_to, format}` (format e.g. id_ed25519)
+-- Run-on-demand step        -> add `only: <tag>`; run with `microCI -O <tag>` (docs 120 CLI help)
+-- Execute subset of steps   -> `microCI -N 3,5` (numbers from `microCI -l`) or `-x hh` (hash)
+-- GUI app in container      -> docs 300_config_display.md (DISPLAY env + /tmp/.X11-unix volume)
```


### Plugin index (docs file ↔ template)

| Plugin | Doc (upstream `docs/`) | Template (upstream `new/`) | Notes |
|--------|------------------------|----------------------------|-------|
| bash | 210_plugin_bash.md | bash.yml | full common-field reference is in bash.yml comments |
| skip | 211_plugin_skip.md | skip.yml | no params required |
| fetch | 212_plugin_fetch.md | fetch.yml | url / git_archive / github item forms |
| git_deploy | 220_plugin_git_deploy.md | git_deploy.yml | needs `ssh:` |
| git_publish | 221_plugin_git_publish.md | git_publish.yml | needs `ssh:` |
| mkdocs_material | 230_plugin_mkdocs_material.md | mkdocs_material.yml | action: build/serve/deploy |
| mkdocs_material_config | 231_plugin_mkdocs_material_config.md | mkdocs_material_config.yml | |
| pandoc | 232_plugin_pandoc.md | pandoc.yml | |
| beamer | 233_plugin_beamer.md | beamer.yml | |
| plantuml | 234_plugin_plantuml.md | plantuml.yml | |
| pikchr | 235_plugin_pikchr.md | pikchr.yml | |
| mermaid | — (template only) | mermaid.yml | CLI `--new mermaid` exists |
| docmd | 236_plugin_docmd.md | docmd.yml | |
| doxygen | 237_plugin_doxygen.md | doxygen.yml | |
| asciidoc | 238_plugin_asciidoc.md | asciidoc.yml | |
| clang-format | 240_plugin_clang-format.md | clang-format.yml | config doc: 241_plugin_clang-format_config.md |
| cpp | 241_plugin_cpp.md | cpp.yml | |
| clang-tidy | 242_plugin_clang-tidy.md | clang-tidy.yml | |
| cppcheck | 243_plugin_cppcheck.md | cppcheck.yml | |
| flawfinder | 244_plugin_flawfinder.md | flawfinder.yml | |
| jfrog | 250_plugin_jfrog.md | jfrog.yml | creds via env |
| minio | 251_plugin_minio.md | minio.yml | creds via env |
| npm | 252_plugin_npm.md | npm.yml | |
| raspberry_pico | 253_plugin_raspberry_pico.md | raspberry_pico.yml | privileged + /dev/bus/usb for load/info |
| docker_build | — (template only) | docker_build.yml | folder/dockerfile/target/version/push |
| vhdl-format | — (template only) | vhdl-format.yml | |
| unsafe | 299_plugin_unsafe.md | unsafe.yml | host execution; restricted field set |

When a doc and a template disagree or one is missing, prefer the **template** for key names
(it is generated from the source code by `microCI --new <plugin>`) and mention the gap to the user.

### Validation (mandatory after every edit)

1. Config test: `microCI -T -i .microCI.yml` — parses and validates the pipeline.
2. Step list: `microCI -l -i .microCI.yml` — confirms step names/numbers as the user will see them.
3. Optional dry inspection of generated script: `microCI -i .microCI.yml | head -n 50`.

Binary resolution order: `microCI` on PATH → repo build `bin/microCI` (build with `make -C src`)
→ ask the user how to run it. If no binary is available, say so explicitly and mark the edit as
"not validated".

## Resources

- Upstream repo: https://github.com/geraldolsribeiro/microCI
- Docs site: https://microci.dev
- Raw docs base: https://raw.githubusercontent.com/geraldolsribeiro/microCI/master/
- CLI reference snapshot: `<cache>/docs/120_help.md` (regenerated from `microCI --help`)
