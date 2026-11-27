#!/usr/bin/env bash
# sync.sh - Refresh the microCI skill docs/templates cache from GitHub.
# Usage: bash sync.sh [cache_dir]   (default: ~/.cache/microci-skill)
# Downloads the repository archive ONCE and extracts only the needed files (fast, 2 requests total).
# Exit code != 0 means the cache must not be trusted.
set -euo pipefail

CACHE_DIR="${1:-${HOME}/.cache/microci-skill}"
OWNER="geraldolsribeiro"
REPO="microCI"
BRANCH="master"
API_BASE="https://api.github.com/repos/${OWNER}/${REPO}"
ARCHIVE_URL="https://codeload.github.com/${OWNER}/${REPO}/tar.gz/refs/heads/${BRANCH}"
UA="microci-skill-sync"

DOCS_DIR="${CACHE_DIR}/docs"
TMPL_DIR="${CACHE_DIR}/templates"

command -v curl >/dev/null || { echo "curl is required" >&2; exit 1; }
command -v tar  >/dev/null || { echo "tar is required"  >&2; exit 1; }

START_TS=$(date +%s)

# Head commit (best effort)
COMMIT="$(curl -fsSL -H "User-Agent: ${UA}" "${API_BASE}/commits/${BRANCH}" \
          | grep -o '"sha": *"[0-9a-f]*"' | head -n1 | sed 's/.*"\([0-9a-f]*\)"/\1/' || echo unknown)"

TMP_TAR="$(mktemp /tmp/microci-skill.XXXXXX.tar.gz)"
TMP_OUT="$(mktemp -d /tmp/microci-skill-extract.XXXXXX)"
cleanup() { rm -f "${TMP_TAR}"; rm -rf "${TMP_OUT}"; }
trap cleanup EXIT

# Single archive download + selective extraction
curl -fsSL -H "User-Agent: ${UA}" -o "${TMP_TAR}" "${ARCHIVE_URL}"
tar -xzf "${TMP_TAR}" -C "${TMP_OUT}"

PREFIX="${REPO}-${BRANCH}"
mkdir -p "${DOCS_DIR}" "${TMPL_DIR}"

DOCS_COUNT=0
while IFS= read -r f; do
    [ -z "${f}" ] && continue
    cp "${f}" "${DOCS_DIR}/$(basename "${f}")"
    DOCS_COUNT=$((DOCS_COUNT + 1))
done < <(find "${TMP_OUT}/${PREFIX}/docs" -maxdepth 1 -type f -name '[0-9][0-9][0-9]_*.md')

TMPL_COUNT=0
while IFS= read -r f; do
    [ -z "${f}" ] && continue
    cp "${f}" "${TMPL_DIR}/$(basename "${f}")"
    TMPL_COUNT=$((TMPL_COUNT + 1))
done < <(find "${TMP_OUT}/${PREFIX}/new" -maxdepth 1 -type f -name '*.yml')

# Sanity lower bounds (upstream grows over time)
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

ELAPSED=$(( $(date +%s) - START_TS ))
echo "microci-skill sync OK: ${DOCS_COUNT} docs, ${TMPL_COUNT} templates in ${ELAPSED}s -> ${CACHE_DIR} (commit ${COMMIT})"


