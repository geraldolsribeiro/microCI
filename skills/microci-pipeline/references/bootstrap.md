# Bootstrap: sync microCI docs & templates from GitHub

The skill ships **without** documentation. Authoritative content is downloaded to a local cache
from the upstream repo <https://github.com/geraldolsribeiro/microCI> (branch `master`).

## Cache layout

```
<cache>/                      # %LOCALAPPDATA%\microci-skill  (Windows)
  docs/                       # ~ / ~/.cache/microci-skill   (Linux/macOS)
    111_quick_start.md ... 300_config_display.md      # all numbered concept/plugin docs
  templates/
    bash.yml ... unsafe.yml                           # all new/*.yml canonical skeletons
  .sync_meta                  # source, branch, commit sha, sync timestamp (UTC ISO-8601)
```

Only these upstream files are synced:
- `docs/<name>.md` where `<name>` starts with three digits (`^\d{3}_.*\.md$`) — this captures the
  concept docs (1xx), all plugin docs (2xx) and config docs (3xx). Drafts (`draft_*`) are skipped.
- `new/*.yml` — canonical step templates (identical to `microCI --new <plugin>` output).

## Method 1 — sync scripts (preferred when a shell is available)

Windows PowerShell:
```powershell
powershell -ExecutionPolicy Bypass -File scripts/sync.ps1
```
Linux/macOS:
```bash
bash scripts/sync.sh
```
Both are idempotent, print what they fetched and write `.sync_meta`. Exit code != 0 means the
cache must NOT be trusted.

## Method 2 — web fetch only (no shell needed)

Any agent with an HTTP fetch tool can perform the sync by itself:

1. **List files** (GitHub contents API, one call per folder):
   - `https://api.github.com/repos/geraldolsribeiro/microCI/contents/docs`
   - `https://api.github.com/repos/geraldolsribeiro/microCI/contents/new`
   The JSON entries contain `name`, `download_url` and (for the API response headers/body) you can
   also read the current commit from any entry's `sha`-bearing `_links`; simpler: fetch
   `https://api.github.com/repos/geraldolsribeiro/microCI/commits/master` for the head SHA.
2. **Filter**: docs entries matching `^\d{3}_.*\.md$`; all `new/` entries ending in `.yml`.
3. **Download** each file from its `download_url` (or build it as
   `https://raw.githubusercontent.com/geraldolsribeiro/microCI/master/docs/<name>` /
   `.../new/<name>`) and save under `<cache>/docs/` or `<cache>/templates/`.
4. **Write `.sync_meta`**, e.g.:
   ```
   source=https://github.com/geraldolsribeiro/microCI
   branch=master
   commit=<head sha>
   synced_at=2026-11-27T11:45:00Z
   docs_count=38
   templates_count=32
   ```

Batch the downloads (multiple fetch calls in parallel) — there are ~40 small files.

## Method 3 — throwaway clone (when git is available but network APIs are blocked)

```bash
git clone --depth 1 --branch master https://github.com/geraldolsribeiro/microCI <tmpdir>
# copy <tmpdir>/docs/{NNN}_*.md -> <cache>/docs/ ; <tmpdir>/new/*.yml -> <cache>/templates/
```

## Freshness policy

- Cache is **stale** when `.sync_meta` is missing, unparseable, `synced_at` older than 24 h, or a
  needed file is absent.
- On stale → resync (any method) before editing pipeline files.
- Resync failure with an existing cache → you may continue using it, but warn the user it is
  out of date.
- Resync failure without any cache → **stop**; tell the user the skill cannot safely edit
  `.microCI.yml` offline. Do not guess keys.

## Verifying a sync succeeded

- `<cache>/docs/210_plugin_bash.md` exists and starts with `# ⚙️ Bash plugin`.
- `<cache>/templates/bash.yml` exists and contains `plugin:` / `name: bash`.
- File counts are > 30 docs and > 25 templates (upstream grows; use as sanity lower bounds).
