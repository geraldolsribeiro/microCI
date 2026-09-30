# AGENTS.md — microCI

> **Purpose of this file:** Give an AI coding agent (or a new human contributor) a fast, accurate map of the
> repository so it can build, test, extend, and debug microCI without rediscovering the architecture.
> Read this top-to-bottom once; use the **Quick Start** and **Common Tasks** sections as a day-to-day reference.

---

## 1. What microCI is

**microCI** is a single-binary C++20 command-line tool that reads a YAML pipeline definition
(`.microCI.yml`) and **generates a plain, portable Bash script**. The generated script is executed
directly (`microCI | bash`) — there is no hidden runtime, no vendor lock-in.

- **Tagline:** *"Write your pipeline once. Execute it anywhere."*
- **Value proposition:** Define CI/CD, release automation, doc generation, container builds, etc. in YAML,
  then run the same pipeline on a laptop, a CI server, a deploy server, or behind a webhook.
- **Author / license:** Geraldo Luis da Silva Ribeiro — **MIT License** (see `LICENSE`).
- **Website:** https://microci.dev
- **Current version:** defined in `include/MicroCI.hpp` as `#define microCI_version "0.50.0"` (the single source of truth — the root `Makefile` extracts it from there).

> **Language note:** the codebase is in C++/English, but some comments and the author's original notes are in
> Brazilian Portuguese. Treat Portuguese comments as legitimate, not as noise.

---

## 2. Quick Start (build / test / run)

All commands are run from the repo root. The build targets are
Linux/macOS (x86_64 and ARM64) **and Windows via MSYS2 / MinGW64** (yields `bin/microCI.exe`).
The *generated* script is always plain Bash, so on Windows you run it with Git Bash / MSYS2 `bash`
(`bin/microCI.exe | bash`).

```bash
# --- Install build dependencies -------------------------------------------
# Debian/Ubuntu:
sudo apt install libyaml-cpp-dev libspdlog-dev libssl-dev libzstd-dev \
                 upx-ucl equivs devscripts git-buildpackage glow xxd g++ make wget
# macOS (Homebrew):
brew install spdlog yaml-cpp inja glow upx coreutils xxd
# Windows (MSYS2 / MinGW64 shell) — produces bin/microCI.exe:
pacman -S --needed make vim mingw-w64-x86_64-gcc \
        mingw-w64-x86_64-yaml-cpp mingw-w64-x86_64-openssl \
        mingw-w64-x86_64-zstd mingw-w64-x86_64-zlib mingw-w64-x86_64-upx

# --- Fetch header-only third-party libs (argh, inja, inicpp, nlohmann/json) --
# (already present under include/3rd/ in the repo; re-download if missing)
make -C src header_only

# --- Build the binary → bin/microCI (Linux/macOS) or bin/microCI.exe (Windows) ---
make -C src            # or: make -C src clean all

# --- Run the test suite ----------------------------------------------------
make -C test           # runs snapshot_create_script + snapshot_cmd_new + snapshot_cmd_external + runtime

# --- Use the tool (on Windows use bin/microCI.exe) -------------------------
bin/microCI --help
bin/microCI --version
bin/microCI                          # reads ./.microCI.yml, prints a Bash script to stdout
bin/microCI | bash                   # generate AND execute the pipeline
bin/microCI --list                   # list steps (number + hash + name)
bin/microCI --number 7,9,11          # generate only specific steps (by 1-based number)
bin/microCI --new bash               # scaffold a new pipeline step from the bash template
```

**Fastest feedback loop for a C++ change:**

```bash
make -C src 2>&1 | tail -n 20        # rebuild (single compilation unit — one big compile)
make -C test snapshot_create_script_test   # run just one snapshot suite
```

---

## 3. Repository Map

```
microCI/
├── AGENTS.md                     ← YOU ARE HERE
├── README.md                     ← generated from docs/index.md (do not hand-edit; see §8)
├── Makefile                      ← top-level: build, test, deb/rpm packaging, publish
├── .microCI.yml                  ← microCI's OWN pipeline (dogfooding). The canonical example.
├── LICENSE, AUTHORS, microci.equivs
├── mkdocs.yml                    ← MkDocs site config
├── git2dch.sh                    ← git log → debian/changelog helper
│
├── src/                          ← C++ implementation (one .cpp per concern)
│   ├── Makefile                  ← THE build system (single compilation unit, asset embedding)
│   ├── main.cpp                  ← CLI entry point, plugin registration, --new/--external/--config
│   ├── MicroCI.cpp               ← core orchestrator (read config, env, docker, script buffer)
│   ├── MicroCIUtils.cpp          ← small helpers (banner, sanitizeName, stepName, replaceAll…)
│   ├── PluginStepParser.cpp      ← abstract base class shared by every plugin
│   ├── ConsoleBox.cpp            ← pretty console error/info/debug boxes
│   ├── .header.cpp               ← template prepended to every generated asset header
│   ├── <Name>PluginStepParser.cpp  ← one file per plugin (see §4.5 for the full list)
│   └── single_compilation_unit.cpp ← GENERATED: #includes every other .cpp (do not edit)
│
├── include/                      ← project headers
│   ├── MicroCI.hpp               ← MicroCI class + version macro
│   ├── PluginStepParser.hpp      ← abstract plugin base class
│   ├── MicroCIUtils.hpp
│   ├── ConsoleBox.hpp
│   ├── <Name>PluginStepParser.hpp
│   ├── new/                      ← GENERATED asset headers (from ../new/*.yml) — DO NOT EDIT
│   ├── help/                     ← GENERATED asset headers (from ../help/*.txt) — DO NOT EDIT
│   ├── sh/                       ← GENERATED asset headers (from ../sh/*.sh) — DO NOT EDIT
│   ├── 3rd/                      ← header-only 3rd-party libs (argh, inja, nlohmann, inicpp)
│   └── external/                 ← GENERATED asset headers (from ../external/*.yml) — DO NOT EDIT
│
├── new/                          ← YAML TEMPLATES used by `microCI --new <plugin>` (source of truth)
│   ├── bash.yml, skip.yml, fetch.yml, minio.yml, jfrog.yml, … (one per plugin)
│   └── gitlab-ci.yml, mkdocs_material_index.md, …
│
├── help/                         ← plain-text help (from `???_plugin_*.md` via glow) — feeds `--help`
│   └── bash.txt, skip.txt, …
│
├── sh/                           ← the Bash script TEMPLATE(s) the tool emits (source of truth)
│   ├── MicroCI.sh                ← the generated-script skeleton (banner, deps check, step runner)
│   └── NotifyDiscord.sh          ← optional Discord webhook notification block
│
├── external/                     ← YAML specs for `microCI --external <lib>` (argh, doctest, fmt, …)
│
├── docs/                         ← MkDocs Material documentation site
│   ├── Makefile                  ← converts ???_plugin_*.md → ../help/*.txt (via glow)
│   ├── index.md                  ← the real README source (README.md is derived from this)
│   ├── 1xx_*.md                  ← intro / install / env-vars / help / activity-diagram
│   ├── 15x_*.md                  ← GitHub / GitLab / Jenkins / server config guides
│   ├── 2xx_plugin_*.md           ← ONE FILE PER PLUGIN (the `???` prefix encodes the number)
│   └── diagrams/                 ← PlantUML / pikchr sources
│
├── test/                         ← test harness (golden-file snapshots + runtime)
│   ├── Makefile                  ← test orchestration (see §7)
│   ├── test_helpers.sh           ← shared JUnit-XML + result helpers
│   ├── snapshot_create_script/<plugin>/{input.yml, expected.sh, test.sh}
│   ├── snapshot_cmd_new/<plugin>/{expected.yml, test.sh}
│   ├── snapshot_cmd_external/<lib>/{expected.yml, test.sh}
│   └── runtime/                  ← end-to-end runtime tests
│
├── dockerfiles/                  ← Docker images, one dir per tool (asciidoc, pandoc, cpp_compiler, …)
│   └── <tool>/{Dockerfile, Makefile, README.md}
│
├── .github/workflows/            ← GitHub Actions
│   ├── c-cpp.yml                 ← build binary (Linux/macOS + Windows) + deb/rpm + pre-release (MAIN CI)
│   ├── docker-image.yml          ← build/push all dockerfiles/* images (multi-arch)
│   └── codeql.yml                ← CodeQL analysis
│
├── debian/                       ← Debian packaging (changelog, control, …)
├── brew/                         ← Homebrew formula (microci.rb)
├── bin/                          ← build output (bin/microCI) + publish scripts
├── 3rd/                          ← a couple of non-header assets (beamercolorthemestr.sty, str-logo.png)
└── .pi/prompts/                  ← author's AI prompt templates (e.g. doc_microci_plugin.md)
```

---

## 4. Core Architecture

### 4.1 The object model (three layers)

```
                 main.cpp
                      │  (parses CLI, registers plugins, loads env)
                      ▼
        ┌───────────────────────────┐
        │         MicroCI           │  ← orchestrator (include/MicroCI.hpp)
        │  - reads .microCI.yml     │
        │  - holds envs / docker    │
        │  - owns the script buffer │  Script() → std::stringstream
        │  - plugin registry (map)  │
        └─────────────┬─────────────┘
                      │  for each step:  parsePluginStep(step)
                      ▼
        ┌───────────────────────────┐
        │     PluginStepParser      │  ← abstract base (include/PluginStepParser.hpp)
        │  - Parse(step)  [virtual] │
        │  - parseVolumes/Envs/…    │  ← shared helpers (see 4.3)
        │  - beginFunction/endFunct │  ← emit a bash function wrapper
        │  - prepareRunDocker       │  ← emit `docker run …`
        └─────────────┬─────────────┘
                      │  inherits
        ┌─────────────┼─────────────┬──────────────────┐
        ▼             ▼             ▼                  ▼
   BashPlugin     SkipPlugin   FetchPlugin     … (one class per plugin)
```

### 4.2 `MicroCI` (the orchestrator)

Defined in `include/MicroCI.hpp`, implemented in `src/MicroCI.cpp`. Responsibilities:

- **`ReadConfig(fileName)`** — loads the YAML, validates it, and drives `parsePluginStep()` for each step.
- **`Script()`** — returns the `std::stringstream` that accumulates the generated Bash. Every plugin
  writes into this buffer.
- **`DefaultDataTemplate()`** — the base `nlohmann::json` context passed to every plugin's inja template
  (defaults: `WORKSPACE=/microci_workspace`, `DOCKER_IMAGE=debian:stable-slim`, `RUN_AS`, ANSI colors,
  `RANDOM_8`, `APPEND_LOG_TEE_FLAG`, …).
- **`DefaultVolumes()` / `DefaultEnvs()` / `DefaultDockerImage()`** — global defaults.
- **`RegisterPlugin(name, parser)`** — maps a plugin name (e.g. `"bash"`) to a `PluginStepParser` instance.
- **`initBash()`** — renders `sh/MicroCI.sh` (the script skeleton) into the buffer first, then each step.
- **Step selection:** `SetOnlyStep`, `SetOnlyStepNumber`, `SetOnlyStepHash` — power `--number` / `--hash`.
- **`List()` / `ActivityDiagram()`** — power `--list` and `--activity-diagram`.

### 4.3 `PluginStepParser` (the base class every plugin extends)

Defined in `include/PluginStepParser.hpp`, implemented in `src/PluginStepParser.cpp`. It owns a
`MicroCI* mMicroCI` back-pointer and exposes the shared machinery a plugin uses to emit a Docker-wrapped
bash function:

| Helper | What it does |
| --- | --- |
| `Parse(step)` | **Override this.** Receives one YAML step node. |
| `beginFunction(data, envs)` / `endFunction(data)` | Emit the opening/closing of a named bash function. |
| `prepareRunDocker(data, envs, volumes)` | Emit the `docker run …` invocation (image, network, devices, run-as, envs, volumes). |
| `parseVolumes(step)` | Merge global + per-step volumes (`source`/`destination`/`mode`, default `ro`). |
| `parseEnvs(step)` | Collect `envs:` map into a `std::set<EnvironmentVariable>`. |
| `parseRunAs(step, data, default)` | Resolve `run_as` (default usually `"user"`). |
| `parseNetwork(step, data, default)` | Resolve `network` (default `"none"`; options `bridge\|host\|none`). |
| `parseDevices(step, data)` | Resolve `devices:` list. |
| `parseSsh(step, data, volumes, envs)` | Handle `ssh:` (mount `~/.ssh` read-only at `/.microCI_ssh`, set `GIT_SSH_COMMAND`). |
| `copySshIfAvailable(step, data)` | Emit the `cp` that copies keys into `ssh.copy_to`. |
| `stepDockerImage(step, override)` | Resolve image with priority: **step `docker:` > plugin default > global default**. |
| `stepDockerWorkspace(step, ws)` | Resolve the workspace path. |
| `invalidConfigurationDetected()` | Flags the run as invalid (still emits a best-effort script). |

### 4.4 How a plugin emits output (the canonical pattern)

Every concrete plugin's `Parse()` follows the same shape (see `src/SkipPluginStepParser.cpp` for the
simplest, `src/BashPluginStepParser.cpp` for a full one):

```cpp
void SomePluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();          // 1. start from the base template
  auto envs = parseEnvs(step);                            // 2. collect envs
  // … read plugin-specific fields from step["plugin"][…] …
  data["STEP_NAME"]        = stepName(step);              // 3. fill inja template vars
  data["STEP_DESCRIPTION"] = stepDescription(step, "…");
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"]     = stepDockerImage(step);

  mMicroCI->Script() << "# <plugin> \n";
  beginFunction(data, envs);                              // 4. open the bash function
  prepareRunDocker(data, envs, volumes);                  // 5. emit `docker run …`
  mMicroCI->Script() << inja::render(R"( … )", data);     // 6. emit plugin-specific commands
  endFunction(data);                                      // 7. close the bash function
}
```

Templates are rendered with **inja** (`3rd/inja.hpp`) against the `nlohmann::json` `data` object.

### 4.5 The full plugin registry

Registered in `src/main.cpp` (`uCI.RegisterPlugin(...)`). The **name** is the string users write in
`plugin.name:` in their YAML.

| YAML `plugin.name` | C++ class | Notes |
| --- | --- | --- |
| `bash` | `BashPluginStepParser` | Run `/bin/bash` or `/bin/sh` commands |
| `skip` | `SkipPluginStepParser` | No-op step (still tracked) |
| `fetch` | `FetchPluginStepParser` | Download files / git archive (largest parser) |
| `git_deploy` | `GitDeployPluginStepParser` | git push / deploy |
| `git_publish` | `GitPublishPluginStepParser` | publish to a git remote |
| `mkdocs_material` | `MkdocsMaterialPluginStepParser` | build/serve docs |
| `pandoc` | `PandocPluginStepParser` | document conversion |
| `beamer` | `BeamerPluginStepParser` | LaTeX PDF presentations |
| `plantuml` | `PlantumlPluginStepParser` | UML diagrams |
| `pikchr` | `PikchrPluginStepParser` | diagrams |
| `mermaid` | `MermaidPluginStepParser` | diagrams |
| `docmd` | `DocmdPluginStepParser` | extract code comments → markdown |
| `doxygen` | `DoxygenPluginStepParser` | API docs |
| `asciidoc` | `AsciidocPluginStepParser` | AsciiDoc rendering |
| `clang-format` | `ClangFormatPluginStepParser` | C/C++ formatting |
| `clang-tidy` | `ClangTidyPluginStepParser` | C/C++ static analysis |
| `cppcheck` | `CppCheckPluginStepParser` | C/C++ static analysis |
| `flawfinder` | `FlawfinderPluginStepParser` | SAST |
| `cpp` | `CppPluginStepParser` | compile C/C++ |
| `vhdl-format` | `VHDLFormatPluginStepParser` | VHDL formatting |
| `minio` | `MinioPluginStepParser` | S3 (MinIO) artifacts |
| `jfrog` | `JFrogPluginStepParser` | JFrog artifacts |
| `npm` | (via `TemplatePluginStepParser`) | npm |
| `raspberry_pico` | `RaspberryPicoPluginStepParser` | RP2040/RP2350 firmware |
| `template` | `TemplatePluginStepParser` | generic template step |
| `unsafe` | `UnsafePluginStepParser` | run a command on the **host** (no Docker) |
| `docker_build` | ⚠️ see **Gotcha G1** | build a Docker image |

> **Note:** `npm` is not in the `--help` list and `docker_build` is registered against the wrong class —
> see **§11 Gotchas**.

---

## 5. The CLI (from `src/main.cpp`)

`microCI` uses the **argh** header-only arg parser. Recognized options (validated against a whitelist —
unknown options are a hard error):

| Option | Meaning |
| --- | --- |
| *(none)* | Read `.microCI.yml` (or `-i FILE`) and print the generated Bash script to stdout |
| `-i, --input FILE` | Use an alternative pipeline file (default `.microCI.yml`) |
| `-O, --only NAME` | Generate only the step with this name |
| `-N, --number 7,9,11` | Generate only steps with these 1-based numbers (order is normalized) |
| `-x, --hash XXXX` | Generate only the step with this 4-hex-digit hash |
| `-l, --list` | List steps (`number hash name`) and exit |
| `-A, --activity-diagram` | Emit a PlantUML activity diagram of the pipeline and exit |
| `-T, --test-config` | Validate the config; exit 0 if valid, 1 otherwise |
| `-a, --append-log` | Append (not truncate) `.microCI.log` |
| `-H, --home PATH` | Alternative global-config home |
| `-U, --update-db` | Update `/opt/microCI/db.json` (step status DB) |
| `-n, --new PLUGIN` | Scaffold a new step from the `new/<plugin>.yml` template into `.microCI.yml` |
| `-c, --config TYPE` | Create a config file from a template (e.g. `gitlab_ci` → `.gitlab-ci.yml`) |
| `-e, --external LIB` | Download a header-only C++ lib (argh, doctest, fmt, jwt-cpp, lyra, nlohmann_json, spdlog) |
| `-h, --help [PLUGIN]` | General help, or per-plugin help (from `help/*.txt`) |
| `-V, --version` | Print version |
| `-u, --update` | Print a shell snippet that self-updates to latest stable |
| `-D, --update-dev` | Print a shell snippet that self-updates to the dev stream |
| `-X, --uninstall` | Print a shell snippet that removes `/usr/bin/microCI` |

**Environment variables:**
- `MICROCI_*` — any env var prefixed `MICROCI_` is injected into the pipeline context.
- A curated allow-list of **GitLab CI** predefined variables (`CI_COMMIT_SHA`, `CI_JOB_TOKEN`, …) is
  also picked up (see `loadGitlabEnvironmentVariables` in `main.cpp`).
- Secrets are conventionally supplied via `~/.microCI.env` (global) and `.env` (project) — both are read
  at generation time and injected.

---

## 6. Build System (the part that surprises people)

The build is driven by **`src/Makefile`**. Two non-obvious mechanisms:

### 6.1 Asset embedding (binary → C header)

YAML templates, help text, and the shell skeleton are **compiled into the binary** so the release is a
single self-contained executable. The pattern rules in `src/Makefile` use `xxd -i` to turn each source
file into a `unsigned char []` array in a generated header:

| Source (edit these) | Generated header (do NOT edit) |
| --- | --- |
| `new/*.yml`, `new/*.md` | `include/new/<name>.hpp` |
| `help/*.txt` | `include/help/<name>.hpp` |
| `sh/*.sh` | `include/sh/<name>.hpp` |
| `3rd/*` | `include/3rd/<name>.hpp` |
| `external/*.yml` | `include/external/<name>.hpp` |

Each generated header starts with the `src/.header.cpp` banner and the comment
`// DON'T EDIT THIS FILE, INSTEAD UPDATE <source>`. They expose `unsigned char ___<dir>_<name>_<ext>[]`
plus a `..._len` symbol.

> **Rule for agents:** If you need to change a `--new` template, edit `new/<plugin>.yml`. If you need to
> change the emitted script skeleton, edit `sh/MicroCI.sh`. If you need to change `--help` text, edit the
> `docs/???_plugin_<name>.md` (regenerated into `help/<name>.txt`). **Never hand-edit anything under
> `include/new/`, `include/help/`, `include/sh/`, or `include/external/`.**

### 6.2 Single compilation unit

`src/single_compilation_unit.cpp` is generated by concatenating `#include` lines for every other `.cpp`
in `src/`, then compiled as **one translation unit**. This enables whole-program dead-code elimination
(`-fdata-sections -ffunction-sections -Wl,--gc-sections`), static linking, and a small UPX-compressed
binary. Consequences:

- A single compile step (slower per build, but simple).
- `main.cpp` is included **last** (it holds `main()`).
- The binary is stripped and (on non-macOS-ARM) compressed with `upx --best`.
- macOS ARM builds skip UPX/static (no UPX for arm64 macOS).

### 6.3 Compiler & linker flags (summary)

- Standard: **C++20** (`-std=c++20`), `-Os`, `-Wall -Wextra -Wpedantic -Werror`.
  **MinGW exception:** `-Os`/`-Oz` break `std::string` linking on MinGW, so the Makefile uses `-O2`
  there (size flags still apply). Output is `bin/microCI.exe` on Windows.
- Links: `-lyaml-cpp -lcrypto -lzstd -lz` (plus `spdlog` header-only via `include/external`).
- Includes: `-I../include -I../include/3rd`.
- Header-only deps live in `include/3rd/` (argh, inja, nlohmann/json, inicpp) and `include/external/`.

### 6.4 Useful `src/Makefile` targets

| Target | Effect |
| --- | --- |
| `all` (default) | Build `../bin/microCI` (or `../bin/microCI.exe` on Windows) |
| `clean` | Remove binary; `touch` sources to force asset-header regeneration |
| `rebuild` | `clean` + rebuild docs |
| `header_only` | Re-download the header-only 3rd-party libs into `include/3rd/` |
| `mac_deps` / `debian_deps` | Install build dependencies |
| `tidy` | Run `clang-tidy` over the sources |
| `diagram` | Emit the PlantUML activity diagram for `.microCI.yml` |

---

## 7. Testing

Orchestrated by **`test/Makefile`**. The suite is **golden-file (snapshot)** based: a fixture is fed to
the real binary, the output is normalized, and diffed against a stored expected file.

```bash
make -C test                                   # run everything
make -C test snapshot_create_script_test       # one suite
make -C test snapshot_cmd_new_test             # one suite
make -C test snapshot_cmd_external_test        # one suite
make -C test runtime_test                      # one suite
```

### 7.1 The three snapshot suites

| Suite | What it verifies | Fixture layout |
| --- | --- | --- |
| `snapshot_create_script/<plugin>/` | `microCI -i input.yml` → generated Bash matches `expected.sh` | `input.yml`, `expected.sh`, `test.sh` |
| `snapshot_cmd_new/<plugin>/` | `microCI --new <plugin>` output matches `expected.yml`, **and** `expected.yml` matches the canonical `new/<plugin>.yml` | `expected.yml`, `test.sh` |
| `snapshot_cmd_external/<lib>/` | `microCI --external <lib>` output matches `expected.yml` | `expected.yml`, `test.sh` |

Each suite has a shared `runner_helper.sh` (invoked by each `<plugin>/test.sh`) and a `test_all.sh` that
auto-discovers plugin dirs, runs them, and writes a **JUnit XML** report (via `test_helpers.sh`).

### 7.2 Output normalization (why snapshots are stable)

`runner_helper.sh` normalizes unstable parts before diffing:
- microCI version → `v9.99.9`
- macOS `uuidgen | head -c 8` → Linux `head -c 8 /proc/sys/kernel/random/uuid`
- temp paths (`/tmp/tmp.XXX/`, `/var/folders/...`) → stripped

### 7.3 Updating a snapshot (when you intentionally change output)

```bash
# After rebuilding bin/microCI with your change:
make -C test snapshot_create_script_update     # regenerate snapshot_create_script expected files
# or, per-suite update scripts:
./test/snapshot_cmd_new/snapshot_update.sh
```

> **Workflow for a behavior change:** make the C++ change → `make -C src` → run the affected suite →
> if the diff is *intended*, regenerate that suite's expected files → re-run to confirm green.

---

## 8. Documentation

- **Site:** MkDocs Material (`mkdocs.yml`, `docs/`). Built by `docs/Makefile`.
- **README:** `README.md` is **derived** from `docs/index.md` (see the "Configure Github pages and README"
  step in `.microCI.yml`). Edit `docs/index.md`, not `README.md`.
- **Per-plugin docs:** one `docs/???_plugin_<name>.md` per plugin (the `???` is a 3-digit sort prefix,
  e.g. `210_plugin_bash.md`).
- **Help text:** `docs/Makefile` renders each `???_plugin_*.md` to `help/<name>.txt` using `glow`
  (forced-color, fixed width). These feed `microCI --help <plugin>`.
- **Author's AI prompt** for writing plugin docs: `.pi/prompts/doc_microci_plugin.md` — a good template
  for the expected doc structure (Overview / Features / Setup & Configuration / Examples).

---

## 9. Conventions & Style

- **C++20, modern idioms.** Prefer `std::format` over `fmt::format` (the codebase was recently migrated).
- **Formatting:** `.clang-format` is authoritative. Run `clang-format` (or the `clang-format` plugin) before
  committing. CI enforces formatting.
- **License header:** every source file carries the MIT header **plus** the microCI ASCII-art banner.
  Keep it when creating new files.
- **Namespace:** everything is in `namespace microci`.
- **Naming:** plugin classes are `<PascalCaseName>PluginStepParser`; files mirror the class name.
- **YAML templates** (`new/*.yml`) use `#{{{` … `#}}}` markers to delimit the real content (consumed by
  the `docmd` tool and the template writer). Keep comments in the template — they become user-facing help.
- **Portuguese is fine** in comments (author's language); English is preferred for new code.
- **No build artifacts committed** — `bin/`, test outputs, etc. are git-ignored. (The generated
  `include/*/*.hpp` asset headers *are* committed because the single-binary build needs them.)

---

## 10. Common Tasks (recipes)

### 10.1 Add a brand-new plugin (the most common extension)

1. **Header** — create `include/<Name>PluginStepParser.hpp` (copy `include/SkipPluginStepParser.hpp` as a
   starting point; class derives from `PluginStepParser`).
2. **Implementation** — create `src/<Name>PluginStepParser.cpp` (copy `src/SkipPluginStepParser.cpp`);
   implement `Parse(const YAML::Node &step)` following the §4.4 pattern.
3. **Register** — add to `src/main.cpp`:
   - `#include "<Name>PluginStepParser.hpp"` (with the other plugin includes)
   - `uCI.RegisterPlugin("<name>", std::make_shared<<Name>PluginStepParser>(&uCI));`
4. **Template** — create `new/<name>.yml` (the `--new` scaffold; keep `#{{{ … #}}}` markers).
5. **Help** — create `docs/???_plugin_<name>.md` (pick the next free 3-digit prefix) so `--help <name>`
   and the docs site pick it up.
6. **Tests** — add fixtures:
   - `test/snapshot_create_script/<name>/{input.yml, expected.sh, test.sh}`
   - `test/snapshot_cmd_new/<name>/{expected.yml, test.sh}`
   (copy an existing simple plugin dir, e.g. `skip`, and adapt.)
7. **Build & verify:** `make -C src` then `make -C test snapshot_create_script_test`.
8. (Optional) **Docker image** — if the plugin needs a toolchain, add `dockerfiles/<name>/{Dockerfile,Makefile,README.md}`.

### 10.2 Change the emitted Bash skeleton

Edit `sh/MicroCI.sh` (the template). Rebuild (`make -C src`) so `include/sh/MicroCI.hpp` regenerates.
Update the affected `snapshot_create_script/*/expected.sh` if the output changed.

### 10.3 Change a `--new` template

Edit `new/<plugin>.yml`. Rebuild. Update `test/snapshot_cmd_new/<plugin>/expected.yml` (it must stay
identical to `new/<plugin>.yml` — the suite asserts that).

### 10.4 Bump the version

Edit `#define microCI_version "X.Y.Z"` in `include/MicroCI.hpp` (single source of truth). The root
`Makefile` and `debian/changelog` derive from it.

### 10.5 Produce a package

```bash
make deb     # Debian .deb (uses equivs + git2dch.sh)
make rpm     # RHEL .rpm (via alien)
```

---

## 11. Gotchas & Known Quirks (read before you touch anything)

- **G1 — `docker_build` is registered to the wrong class.** In `src/main.cpp`, the line
  `uCI.RegisterPlugin("docker_build", std::make_shared<DocmdPluginStepParser>(&uCI));` uses
  `DocmdPluginStepParser` even though a dedicated `DockerBuildPluginStepParser` exists
  (`src/DockerBuildPluginStepParser.cpp`, `include/DockerBuildPluginStepParser.hpp`). If you are working
  on Docker-image-building steps, verify which class is actually intended before "fixing" or relying on it.
- **G2 — `npm` has no registered parser / no `--help` entry.** It appears in templates and docs but is not
  in the `RegisterPlugin` list or the `MICROCI_HELP` macro block. Don't assume `--new npm` behaves like the
  others.
- **G3 — Generated headers are committed.** `include/new|help|sh|external/*.hpp` are build outputs that are
  checked in (needed for the single-binary build). Editing them by hand will be overwritten on the next
  build — always edit the source (`new/`, `help/`, `sh/`, `external/`).
- **G4 — `single_compilation_unit.cpp` is generated.** Do not edit it; it is regenerated from the other
  `.cpp` files. `main.cpp` is deliberately included last.
- **G5 — `-Werror` is on.** Any new compiler warning is a build failure. Keep includes tidy and avoid
  unused variables/parameters (use `[[maybe_unused]]` where needed).
- **G6 — Windows builds use MSYS2 / MinGW64 (native, not WSL).** The build now targets Windows
  natively via the MinGW-w64 GCC toolchain, producing `bin/microCI.exe`. MinGW-specific quirks:
  (a) **`-Os`/`-Oz` break linking** (mis-generate the `std::string` move-ctor reference) — the Makefile
  switches to `-O2` on MinGW; (b) **the linker appends `.exe`** — all target paths use `$(EXE_EXT)`;
  (c) set `git config --global core.autocrlf input` before `make` (CRLF in the Makefile breaks it).
  See `build_report.md` for the full write-up.
- **G7 — Step numbers are 1-based** in `--number` / `--list`, but the internal `MICROCI_STEP_NUMBER`
  counter in the generated script is 0-based. Off-by-one is a recurring trap.
- **G8 — Docker image priority** is `step.docker` > plugin default > global default
  (`debian:stable-slim`). Forgetting to pass a plugin default can silently fall back to the global image.
- **G9 — The project dogfoods itself.** `.microCI.yml` at the repo root is microCI's own pipeline. When you
  change generation behavior, the project's own pipeline (and its snapshots under `test/`) are the first
  things to break.
- **G10 — `make clean` in `src/` touches sources** to force asset-header regeneration. If a template change
  doesn't seem to take effect, run `make -C src clean` first.

---

## 12. Pointers for deeper reading

| Question | Start here |
| --- | --- |
| How is a step turned into Bash? | `src/PluginStepParser.cpp` → `src/BashPluginStepParser.cpp` → `sh/MicroCI.sh` |
| Where are plugins wired up? | `src/main.cpp` (search `RegisterPlugin`) |
| What are the defaults (image, workspace, envs)? | `src/MicroCI.cpp` (`DefaultDataTemplate`, `DefaultVolumes`) |
| How is the binary built so small? | `src/Makefile` (Sections 3, 10, 11) |
| How do I build on Windows? | `build_report.md` + `src/Makefile` (`IS_MINGW`, `EXE_EXT`) |
| How do tests stay stable across OSes? | `test/snapshot_create_script/runner_helper.sh` |
| What is the canonical pipeline example? | `.microCI.yml` (repo root) |
| How to write plugin docs? | `.pi/prompts/doc_microci_plugin.md` + any `docs/2xx_plugin_*.md` |

---

*Keep this file current: when the architecture, build, or test harness changes, update the matching section
so the next agent starts from an accurate map.*