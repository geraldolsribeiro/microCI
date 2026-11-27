#!/usr/bin/env bash
# sync.sh - Refresh the microCI skill docs/templates cache from GitHub.
# Usage: bash sync.sh [cache_dir]   (default: ~/.cache/microci-skill)
# Exit code != 0 means the cache must not be trusted.
set -euo pipefail

CACHE_DIR="${1:-${HOME}/.cache/microci-skill}"
OWNER="geraldolsribeiro"
REPO="microCI"
BRANCH="master"
API_BASE="https://api.github.com/repos/${OWNER}/${REPO}"
RAW_BASE="https://raw.githubusercontent.com/${OWNER}/${REPO}/${BRANCH}"
UA="microci-skill-sync"

DOCS_DIR="${CACHE_DIR}/docs"
TMPL_DIR="${CACHE_DIR}/templates"
mkdir -p "${DOCS_DIR}" "${TMPL_DIR}"

command -v curl >/dev/null || { echo "curl is required" >&2; exit 1; }

# Head commit (best effort)
COMMIT="$(curl -fsSL -H "User-Agent: ${UA}" "${API_BASE}/commits/${BRANCH}" \
          | grep -o '"sha": *"[0-9a-f]*"' | head -n1 | sed 's/.*"\([0-9a-f]*\)"/\1/' || echo unknown)"

DOCS_COUNT=0
TMPL_COUNT=0

while IFS= read -r name; do
    [ -z "${name}" ] && continue
    curl -fsSL -H "User-Agent: ${UA}" -o "${DOCS_DIR}/${name}" "${RAW_BASE}/docs/${name}"
    DOCS_COUNT=$((DOCS_COUNT + 1))
done < <(curl -fsSL -H "User-Agent: ${UA}" "${API_BASE}/contents/docs?ref=${BRANCH}" \
         | grep -o '"name": *"[0-9][0-9][0-9]_[^"]*\.md"' | sed 's/.*"\([^"]*\)"/\1/')

while IFS= read -r name; do
    [ -z "${name}" ] && continue
    curl -fsSL -H "User-Agent: ${UA}" -o "${TMPL_DIR}/${name}" "${RAW_BASE}/new/${name}"
    TMPL_COUNT=$((TMPL_COUNT + 1))
done < <(curl -fsSL -H "User-Agent: ${UA}" "${API_BASE}/contents/new?ref=${BRANCH}" \
         | grep -o '"name": *"[^"]*\.yml"' | sed 's/.*"\([^"]*\)"/\1/')

if [ "${DOCS_COUNT}" -lt 30 ] || [ "${TMPL_COUNT}" -lt 25 ]; then
    echo "microci-skill sync FAILED: sanity check docs=${DOCS_COUNT} templates=${TMPL_COUNT}" >&2
    exit 1
fi

{
    echo "source=https://github.com/${OWNER}/${REPO}"
    echo "branch=${BRANCH}"
    echo "commit=${COMMIT}"
    echo "synced_at=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "docs_count=${DOCS_COUNT}"
    echo "templates_count=${TMPL_COUNT}"
} > "${CACHE_DIR}/.sync_meta"

echo "microci-skill sync OK: ${DOCS_COUNT} docs, ${TMPL_COUNT} templates -> ${CACHE_DIR} (commit ${COMMIT})"
