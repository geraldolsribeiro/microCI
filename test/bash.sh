#!/bin/bash
# ----------------------------------------------------------------------
# DON'T EDIT THIS FILE!
# ----------------------------------------------------------------------
# This bash script was generated by microCI v0.35.0   
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
  echo -e "[0;34m┃                            microCI v0.35.0                         ┃[0m"
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
  || { echo -e "[0;31mThe utility jq was not found in the system[0m";
       echo "[0;31mTry: [0;32msudo apt install jq";
       exit 1; }

command -v yq &> /dev/null \
  || { echo -e "[0;31mThe utility yq was not found in the system[0m";
       echo "[0;31mTry: [0;32msudo snap install yq";
       exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mThe utility curl was not found in the system[0m";
       exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mThe utility docker was not found in the system[0m";
       exit 1; }

# Path without linefeed
MICROCI_PWD=$(pwd -P | tr -d '\n')
export MICROCI_PWD

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

    yq -r .steps[].name "../new/bash.yml" \
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
# bash 

# ----------------------------------------------------------------------
# Description of this step
# ----------------------------------------------------------------------
function step_build_static_version_of_microci() {
  local SECONDS=0
  local MICROCI_STEP_NAME="Build static version of microCI"
  local MICROCI_STEP_DESCRIPTION="Description of this step"
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
  local ENV1="value of the environment variable ENV1"
  local ENV2="value of the environment variable ENV2"
  local ENV_1="1"
  local ENV_2="String with spaces"
  local ENV_YML_1="1"
  local ENV_YML_2="String with spaces"

  {
    (
      set -e

      echo ""
      echo ""
      echo ""
      echo "Step: Build static version of microCI"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --device /dev/dri/ \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_build_static_version_of_microci \
        --network bridge \
        --workdir /microci_workspace \
        --env ENV1="value of the environment variable ENV1" \
        --env ENV2="value of the environment variable ENV2" \
        --env ENV_1="1" \
        --env ENV_2="String with spaces" \
        --env ENV_YML_1="1" \
        --env ENV_YML_2="String with spaces" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "gcc:13" \
        /bin/bash -c "cd /microci_workspace \
           && apt update 2>&1 \
           && apt upgrade -y 2>&1 \
           && apt install -y xxd libspdlog-dev libyaml-cpp-dev 2>&1 \
           && make -C src clean all 2>&1 \
           && make -C test 2>&1 \
           && chown $(id -u).$(id -g) -Rv src/microCI 2>&1"

    )

    status=$?
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
    echo "Duration: ${MICROCI_STEP_DURATION}"
  } >> .microCI.log

  # Notification at the terminal
  if [ "${MICROCI_STEP_SKIP}" = "yes" ]
  then
    echo -e "[0;34mSKIP[0m"
    setStepStatusSkipJson
  elif [ "${status}" = "0" ]
  then
    echo -e "[0;32mOK[0m"
    setStepStatusOkJson
  else
    echo -e "[0;31mFAILED[0m"
    setStepStatusFailJson
    echo "See the complete log at .microCI.log"
    tail -50 .microCI.log
  fi

  ((++MICROCI_STEP_NUMBER))
}
# Update docker images used in the steps
if docker image inspect debian:stable-slim > /dev/null 2>&1 ; then
  echo 'Docker image debian:stable-slim is already updated' >> .microCI.log
else
  echo 'Updating debian:stable-slim docker image...'
  docker pull debian:stable-slim 2>&1 >> .microCI.log
fi
if docker image inspect gcc:13 > /dev/null 2>&1 ; then
  echo 'Docker image gcc:13 is already updated' >> .microCI.log
else
  echo 'Updating gcc:13 docker image...'
  docker pull gcc:13 2>&1 >> .microCI.log
fi


# Execute all steps in the pipeline
function main() {
  date >> .microCI.log

  step_build_static_version_of_microci

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

