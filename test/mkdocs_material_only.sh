#!/bin/bash

set -o posix
shopt -so pipefail

exec 5> .microCI.dbg
BASH_XTRACEFD="5"
PS4='$LINENO: '

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
  echo -e "[0;34m┃                            microCI 0.22.0                          ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

# Verifica se as dependências estão presentes
command -v jq &> /dev/null \
  || { echo -e "[0;31mComando jq não foi encontrado[0m";  exit 1; }

command -v yq &> /dev/null \
  || { echo -e "[0;31mComando yq não foi encontrado[0m";  exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mComando curl não foi encontrado[0m";  exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mComando docker não foi encontrado[0m";  exit 1; }

MICROCI_PWD=$(pwd -P | tr -d '\n')
MICROCI_DB_JSON=/opt/microCI/db.json
MICROCI_STEP_NUMBER=0

function gitOrigin {
  git config --get remote.origin.url || echo "SEM GIT ORIGIN"
}

function pwdRepoId {
  # chave json não pode começar com número
  echo "_$(echo "${MICROCI_PWD}" | md5sum)" | cut -b 1-7
}

function microCI_latest_download_URL_with_version {
  curl -s https://api.github.com/repos/geraldolsribeiro/microci/releases/latest \
    | grep browser_download_url \
    | grep -o -E "https://github.com/geraldolsribeiro/microCI/releases/download/(.*)/microCI"
}

function microCI_download_latest_binary {
  curl -fsSL github.com/geraldolsribeiro/microci/releases/latest/download/microCI -o /usr/local/bin/microCI
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
    echo $( jq --arg name   "$name"   ".repos.$repoId.steps[$stepNum].name   = (\$name)"   ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --argjson ts "$ts"     ".repos.$repoId.steps[$stepNum].ts     = (\$ts)"     ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg status "$status" ".repos.$repoId.steps[$stepNum].status = (\$status)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
  fi
}

function resetStepStatusesJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    local stepNum=0
    echo $( jq --arg origin "$(gitOrigin)" ".repos.$(pwdRepoId).origin = (\$origin)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg status "unknown" ".repos.$(pwdRepoId).status = (\$status)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg path "${MICROCI_PWD}" ".repos.$(pwdRepoId).path = (\$path)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}

    yq -r .steps[].name .microCI.yml \
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
    jq --sort-keys . ${MICROCI_DB_JSON} > /tmp/$$.json && cat /tmp/$$.json > ${MICROCI_DB_JSON}; rm -f /tmp/$$.json
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


# Notificação via Discord não será possível
