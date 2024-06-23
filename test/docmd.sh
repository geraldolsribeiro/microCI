#!/bin/bash
# ----------------------------------------------------------------------
# DON'T EDIT THIS FILE!
# ----------------------------------------------------------------------
# This bash script was generated by microCI v0.31.0   
# based on the configuration files:
# - .microCI.yml Pipeline steps
# - .env         Environment variables, secrets, and other non-versioned information
#
# The microCI documentation can be found at the website <https://microci.dev>.
#
# ----------------------------------------------------------------------
# Summary
# ----------------------------------------------------------------------
# The folloing steps are performed in this script:


# ----------------------------------------------------------------------
# Project banner
# ----------------------------------------------------------------------
{
  echo ""
  echo ""
  echo ""
  echo -e "[0;34m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░█▀▀░▀█▀░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░█░█░█░░░░█░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░█▀▀░▀▀▀░▀▀▀░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░▀░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                            microCI v0.31.0                         ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

# ----------------------------------------------------------------------
# Bash configuration and redirections
# ----------------------------------------------------------------------
set -o posix
shopt -so pipefail

exec 5> .microCI.dbg
BASH_XTRACEFD="5"
PS4='Line $LINENO: '

# ----------------------------------------------------------------------
# Check if the dependencies are installed at the workstation
# ----------------------------------------------------------------------
command -v jq &> /dev/null \
  || { echo -e "[0;31mThe utility jq was not found in the system[0m"; exit 1; }

command -v yq &> /dev/null \
  || { echo -e "[0;31mThe utility yq was not found in the system[0m"; exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mThe utility curl was not found in the system[0m"; exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mThe utility docker was not found in the system[0m"; exit 1; }

# Path without linefeed
MICROCI_PWD=$(pwd -P | tr -d '\n')
MICROCI_DB_JSON=/opt/microCI/db.json
MICROCI_STEP_NUMBER=0

function gitOrigin {
  git config --get remote.origin.url || echo "GIT ORIGIN NOT FOUND!"
}

function pwdRepoId {
  # Como as chaves no json não podem começar com número foi prefixado com underline
  echo "_$(echo "${MICROCI_PWD}" | md5sum)" | cut -b 1-7
}

function microCI_latest_download_URL_with_version {
  curl -s https://api.github.com/repos/geraldolsribeiro/microci/releases/latest \
    | grep browser_download_url \
    | grep -o -E "https://github.com/geraldolsribeiro/microCI/releases/download/(.*)/microCI"
}

function microCI_download_latest_binary {
  curl -fsSL github.com/geraldolsribeiro/microci/releases/latest/download/microCI -o /usr/bin/microCI
}

function updateStepStatusJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    local repoId=$1  ; shift
    local stepNum=$1 ; shift
    local status=$1  ; shift
    local name=$1    ; shift
    local ts
    #local id
    ts=$(date +%s)
    # id=$( echo "${repoId} ${stepName}" | md5sum | sed "s/^[0-9]\+//" | cut -b 1-6)
    # echo $( jq --arg id     "$id"     ".repos.$repoId.steps[$stepNum].id     = (\$id)"     ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    jq --arg name   "$name"   ".repos.$repoId.steps[$stepNum].name   = (\$name)"   ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}
    jq --argjson ts "$ts"     ".repos.$repoId.steps[$stepNum].ts     = (\$ts)"     ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}
    jq --arg status "$status" ".repos.$repoId.steps[$stepNum].status = (\$status)" ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}
  fi
}

function resetStepStatusesJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    local stepNum=0
    jq --arg origin "$(gitOrigin)" ".repos.$(pwdRepoId).origin = (\$origin)" ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}
    jq --arg status "unknown" ".repos.$(pwdRepoId).status = (\$status)" ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}
    jq --arg path "${MICROCI_PWD}" ".repos.$(pwdRepoId).path = (\$path)" ${MICROCI_DB_JSON} \
      > /tmp/$$.tmp && mv /tmp/$$.tmp ${MICROCI_DB_JSON}

    rm -f /tmp/$$.json

    yq -r .steps[].name "../new/docmd.yml" \
      | while IFS= read -r stepName
        do
          updateStepStatusJson "$(pwdRepoId)" "${stepNum}" "unknown" "${stepName}"
          ((++stepNum))
        done
  fi
}

function setStepStatusOkJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    updateStepStatusJson "$(pwdRepoId)" "${MICROCI_STEP_NUMBER}" "OK" "${MICROCI_STEP_NAME}"
  fi
}

function setStepStatusFailJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    updateStepStatusJson "$(pwdRepoId)" "${MICROCI_STEP_NUMBER}" "FAIL" "${MICROCI_STEP_NAME}"
  fi
}

function setStepStatusSkipJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    updateStepStatusJson "$(pwdRepoId)" "${MICROCI_STEP_NUMBER}" "SKIP" "${MICROCI_STEP_NAME}"
  fi
}

function reformatJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    jq --sort-keys . ${MICROCI_DB_JSON} \
      > /tmp/$$.json && cat /tmp/$$.json > ${MICROCI_DB_JSON}
    rm -f /tmp/$$.json
  fi
}

function assert_fail() {
  # Print assert errors to stderr!
  echo "assert failed: \"$*\"" >&2
  _backtrace >&2

  # And crash immediately.
  kill -s USR1 ${TOP_PID}
}

function assert() {
  if [ $# -ne 1 ]
  then
    assert_fail "assert called with wrong number of parameters!"
  fi

  if [ ! "$1" ]
  then
    assert_fail "$1"
  fi
}

function assert_eq() {
  if [ $# -ne 2 ]
  then
    assert_fail "assert_eq called with wrong number of parameters!"
  fi

  assert "${1} -eq ${2}"
}

function assert_function() {
  if [ $# -ne 1 ]
  then
    assert_fail "assert_function called with wrong number of parameters!"
  fi

  local func=$1
  assert "\"$(type -t ${func})\" == \"function\""
}

resetStepStatusesJson
reformatJson


# Notification by Discord is not possible

# ----------------------------------------------------------------------
# Extract documentation from source code to a markdown file
# ----------------------------------------------------------------------
function step_extract_documentation() {
  local SECONDS=0
  local MICROCI_STEP_NAME="Extract documentation"
  local MICROCI_STEP_DESCRIPTION="Extract documentation from source code to a markdown file"
  local MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "GIT ORIGIN NOT FOUND" )
  local MICROCI_GIT_COMMIT_SHA=$( git rev-parse --short HEAD || echo "GIT COMMIT HASH NOT FOUND")
  local MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )
  local MICROCI_STEP_STATUS=":ok:"
  local MICROCI_STEP_SKIP="no"
  local MICROCI_STEP_DURATION=$SECONDS

  # Make step line with 60 characters
  local title="$(( MICROCI_STEP_NUMBER + 1 )) ${MICROCI_STEP_NAME}.............................................................."
  local title=${title:0:60}

  echo -ne "[0;36m${title}[0m: "
  local DOCMD_DETAILS="false"
  local DOCMD_SHOW_BANNER="true"
  local DOCMD_SHOW_SOURCE="false"
  local DOCMD_TOC="false"

  {
    (
      set -e

      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/bash.yml docs/plugin_bash.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/beamer.yml docs/plugin_beamer.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/clang-format_config.yml docs/plugin_clang-format_config.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/clang-format.yml docs/plugin_clang-format.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/clang-tidy.yml docs/plugin_clang-tidy.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/cppcheck.yml docs/plugin_cppcheck.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/fetch2.yml docs/plugin_fetch2.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/fetch.yml docs/plugin_fetch.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/git_deploy.yml docs/plugin_git_deploy.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/git_publish.yml docs/plugin_git_publish.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/minio.yml docs/plugin_minio.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/mkdocs_material_config.yml docs/plugin_mkdocs_material_config.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/mkdocs_material.yml docs/plugin_mkdocs_material.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/npm.yml docs/plugin_npm.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/pandoc.yml docs/plugin_pandoc.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/plantuml.yml docs/plugin_plantuml.md
      echo ""
      echo ""
      echo ""
      echo "Step: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_extract_documentation \
        --network none \
        --workdir /microci_workspace \
        --env DOCMD_DETAILS="false" \
        --env DOCMD_SHOW_BANNER="true" \
        --env DOCMD_SHOW_SOURCE="false" \
        --env DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.3" yaml new/skip.yml docs/plugin_skip.md
    )

    status=$?
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
    echo "Duration: ${MICROCI_STEP_DURATION}"
  } >> .microCI.log

  # Notificação no terminal
  if [ "${MICROCI_STEP_SKIP}" = "yes" ]
  then
    echo -e "[0;34mSKIP[0m"
    setStepStatusSkipJson
  elif [ "${status}" = "0" ]
  then
    echo -e "[0;32mOK[0m"
    setStepStatusOkJson
  else
    echo -e "[0;31mFALHOU[0m"
    setStepStatusFailJson
    echo "See the complete log from .microCI.log"
    tail -50 .microCI.log
  fi

  ((++MICROCI_STEP_NUMBER))
}
# Update docker images used in the steps
echo 'Updating debian:stable-slim docker image ...'
if docker image inspect debian:stable-slim > /dev/null 2>&1 ; then
  echo 'Docker image debian:stable-slim is already updated' >> .microCI.log
else
  docker pull debian:stable-slim 2>&1 >> .microCI.log
fi
echo 'Updating intmain/microci_docmd:0.3 docker image ...'
if docker image inspect intmain/microci_docmd:0.3 > /dev/null 2>&1 ; then
  echo 'Docker image intmain/microci_docmd:0.3 is already updated' >> .microCI.log
else
  docker pull intmain/microci_docmd:0.3 2>&1 >> .microCI.log
fi


# Execute all steps in the pipeline
function main() {
  date >> .microCI.log

  step_extract_documentation

  date >> .microCI.log
}

# Entry point
main

# Usage
# -----
#
# To execute this workflow inside a terminal use the following command:
# microCI | bash
#
# To save the workflow as a bash scritp just redirect the output to a file:
# microCI > build.sh

