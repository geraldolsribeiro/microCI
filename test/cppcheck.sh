#!/bin/bash
# ----------------------------------------------------------------------
# DON'T EDIT THIS FILE!
# ----------------------------------------------------------------------
# This bash script was generated by microCI v0.38.0   
# based on the configuration files:
# - .microCI.yml   Pipeline steps
# - ~/.microCI.env Global environment variables, secrets, and other non-versioned information
# - .env           Project environment variables, secrets, and other non-versioned information
#
# The microCI documentation can be found at the website <https://microci.dev>.
#
# ----------------------------------------------------------------------
# Summary
# ----------------------------------------------------------------------
# The folloing steps are performed in this script:


# Path without linefeed
MICROCI_PWD=$(pwd -P | tr -d '\n')
export MICROCI_PWD

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
  echo -e "[0;34m┃                            microCI v0.38.0                         ┃[0m"
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
       echo "[0;31mTry: [0;32msudo apt install yq";
       echo "[0;31mTry: [0;32msudo snap install yq";
       exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mThe utility curl was not found in the system[0m";
       exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mThe utility docker was not found in the system[0m";
      echo "See page 'Install Docker Engine on Ubuntu' at";
      echo "https://docs.docker.com/engine/install/ubuntu/";
      exit 1; }

groups | grep -q docker || {
  echo -e "[0;31mPlease finish the docker installation adding your user to the docker group[0m"
  exit 1; }

if [ ! -d ~/.ssh ]; then
  echo -e "[0;31mPlease setup the SSH before use microCI[0m"
  echo "The ~/.ssh folder not found"
  exit 1
fi

if [ ! -f ~/.ssh/config ]; then
  echo -e "[0;31mPlease setup the SSH before use microCI[0m"
  echo "The ~/.ssh/config file not found"
  echo "Use the ~/.ssh/config file to pass options to hosts"
  exit 1
fi

if [ ! -f ~/.ssh/id_rsa.pub -a ! -f ~/.ssh/id_ed25519.pub ]; then
  echo -e "[0;31mPlease setup the SSH before use microCI[0m"
  echo "The ~/.ssh/id_rsa.pub file not found"
  echo "The ~/.ssh/id_ed25519.pub file not found"
  echo "Use the ssh-keygen command to setup your pair of keys"
  echo "Consider to use ED25519 SSH keys"
  echo "The book Practical Cryptography With Go suggests that ED25519 keys are more secure and performant than RSA keys."
  echo "OpenSSH 6.5 introduced ED25519 SSH keys in 2014, and they should be available on most operating systems."
  exit 1
fi

if [ ! -f ~/.ssh/known_hosts ]; then
  echo -e "[0;31mPlease setup the SSH before use microCI[0m"
  echo "The ~/.ssh/known_hosts file not found"
  echo "Add your SSH public key to your git server, and"
  echo "clone one of your projects using the SSH protocol"
  echo "git clone git@your.git.server:/some/project.git /tmp"
  exit 1;
fi

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

    yq -r .steps[].name "../new/cppcheck.yml" \
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

MICROCI_STEP_NUMBER=0


# Notification by Discord is not possible

# ----------------------------------------------------------------------
# Check C++ code and generate HTML report
# ----------------------------------------------------------------------
function step_create_c___code_verification_report___cppcheck() {
  local SECONDS=0
  local MICROCI_STEP_NAME="Create C++ code verification report - cppcheck"
  local MICROCI_STEP_DESCRIPTION="Check C++ code and generate HTML report"
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
      echo "Step: Create C++ code verification report - cppcheck"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --user $(id -u):$(id -g) \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_create_c___code_verification_report___cppcheck_$(head -c 8 /proc/sys/kernel/random/uuid) \
        --network none \
        --workdir /microci_workspace \
        --env ENV_1="1" \
        --env ENV_2="String with spaces" \
        --env ENV_YML_1="1" \
        --env ENV_YML_2="String with spaces" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_cpp_compiler:latest" \
        /bin/bash -c "cd /microci_workspace \
        && mkdir -p auditing/cppcheck \
        && rm -f auditing/cppcheck/*.html \
        && cppcheck \
          --platform=unix64 \
          --std=c++11 \
          --enable=all \
          --inconclusive \
          --xml \
          --xml-version=2 \
          --include=include \
          src \
          test \
          2> auditing/cppcheck/cppcheck.xml \
        && cppcheck-htmlreport \
          --title='MicroCI::CppCheck' \
          --report-dir='auditing/cppcheck/' \
          --source-dir='.' \
          --file='auditing/cppcheck/cppcheck.xml' 2>&1"

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
    exit 1
  fi

  ((++MICROCI_STEP_NUMBER))
}
echo 'Updating docker images...'
  echo 'Updating debian:stable-slim docker image...' >> .microCI.log
  docker pull debian:stable-slim --quiet 2>&1 >> .microCI.log
  echo 'Updating intmain/microci_cpp_compiler:latest docker image...' >> .microCI.log
  docker pull intmain/microci_cpp_compiler:latest --quiet 2>&1 >> .microCI.log


# Execute all steps in the pipeline
function main() {
  date >> .microCI.log

  step_create_c___code_verification_report___cppcheck

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

