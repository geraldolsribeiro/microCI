# microCI YAML — stable core schema

Small, stable subset that is safe to rely on before/without a sync. Everything plugin-specific
must still be read from the synced cache (docs + templates). Verified against upstream source:
`src/MicroCI.cpp`, `src/PluginStepParser.cpp`, `new/bash.yml`.

## File skeleton

```yaml
---
docker: "debian:stable-slim"   # optional global default image for all steps
envs:                          # optional global env vars (map)
  GLOBAL_VAR: "value"
steps:                         # required; a list (a map of steps also parses)
  - name: "Step title"         # required, unique-ish human label
    description: "..."         # optional
    # ... common fields below ...
    plugin:                    # required
      name: bash               # required; exact plugin id string
      # ... plugin-specific fields (read docs/templates!) ...
```

## Step-level common fields

| Field | Type | Default | Notes |
|-------|------|---------|-------|
| `name` | string | — (required) | shown by `-l`, selected by `-O` via `only` tag not name |
| `description` | string | — | free text, also stored in observability db |
| `docker` | string | global `docker:` or `debian:stable-slim` | image used for the step container |
| `network` | `none` \| `bridge` \| `host` | `none` | `bridge`/`host` when step needs network (fetch, apt, git push) |
| `run_as` | `root` \| `user` | `user` in templates | user id inside container |
| `uid` / `gid` | string | host `$(id -u)` / `$(id -g)` | custom ownership for outputs |
| `envs` | map | — | values may reference `${VAR}` from `.env`, `.env.yml`, `$HOME/.microCI.env` |
| `volumes` | list of `{source, destination, mode}` | — | `mode: rw` (or ro); host paths mounted in container |
| `devices` | list of strings | — | e.g. `"/dev/dri/"`; passed to docker |
| `privileged` | bool | `false` | needed for picotool USB access |
| `ssh` | map `{copy_from, copy_to, format}` | — | inject SSH keys; `format: "id_ed25519"` etc.; with only `format`, key is mounted from default location for git ops |
| `only` | string tag | — | step runs ONLY via `microCI -O <tag>`; otherwise skipped in normal runs |

Notes:
- The workspace inside the container is `/microci_workspace` (repo root is mounted there).
- Generated script is plain Bash: `microCI | bash`; inspect it with `microCI > /tmp/script.sh`.
- `unsafe` plugin = run on host; forbids `docker`, `network`, `volumes`, `run_as`, `devices`, `ssh`.

## Useful CLI flags (full list: `<cache>/docs/120_help.md`)

| Flag | Purpose |
|------|---------|
| `-T, --test-config` | Validate the pipeline config (use after every edit) |
| `-l, --list` | List steps with numbers |
| `-i, --input <file.yml>` | Use another pipeline file instead of `.microCI.yml` |
| `-O, --only <tag>` | Execute only the step whose `only:` equals `<tag>` |
| `-N, --number N[,N...]` | Execute specific numbered steps (from `-l`) |
| `-n, --new <plugin>` | Print a canonical step skeleton for `<plugin>` to `.microCI.yml` |
| `-A, --activity-diagram` | Generate activity diagram of the pipeline |

## Environment files (loaded automatically)

| File | Format |
|------|--------|
| `$HOME/.microCI.env` | `VAR=VALUE` |
| `.env` (project root) | `VAR=VALUE` |
| `.env.yml` (project root) | `VAR: VALUE` |

Secrets (tokens, credentials for minio/jfrog/git) are referenced from these files, e.g.
`token: "${GIT_TOKEN}"` — never paste literal secrets into `.microCI.yml`.
