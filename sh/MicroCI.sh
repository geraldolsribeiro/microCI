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
  echo -e "{{BLUE}}┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓{{CLEAR}}"
  echo -e "{{BLUE}}┃                                                                    ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░█▀▀░▀█▀░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░█░█░█░░░░█░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░█▀▀░▀▀▀░▀▀▀░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░▀░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                            microCI {{ VERSION }}                      ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                           Geraldo Ribeiro                          ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                                                                    ┃{{CLEAR}}"
  echo -e "{{BLUE}}┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛{{CLEAR}}"
  echo ""
  echo ""
} | tee .microCI.log

# Verifica se as dependências estão presentes
command -v jq &> /dev/null \
  || { echo -e "{{RED}}Comando jq não foi encontrado{{CLEAR}}";  exit 1; }

command -v yq &> /dev/null \
  || { echo -e "{{RED}}Comando yq não foi encontrado{{CLEAR}}";  exit 1; }

command -v curl &> /dev/null \
  || { echo -e "{{RED}}Comando curl não foi encontrado{{CLEAR}}";  exit 1; }

command -v docker &> /dev/null \
  || { echo -e "{{RED}}Comando docker não foi encontrado{{CLEAR}}";  exit 1; }

PWD=$(pwd)

MICROCI_DB_JSON=/opt/microCI/db.json
MICROCI_STEP_NUMBER=0

function gitOrigin {
  git config --get remote.origin.url || echo "SEM GIT ORIGIN"
}

function gitRepoId {
  # id deve começar por letra
  gitOrigin | md5sum | sed "s/^[0-9]\+//" | cut -b 1-6
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
    local id
    ts=$(date +%s)
    id=$( echo "${repoId} ${stepName}" | md5sum | sed "s/^[0-9]\+//" | cut -b 1-6)
    echo $( jq --arg id     "$id"     ".repos.$repoId.steps[$stepNum].id     = (\$id)"     ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg name   "$name"   ".repos.$repoId.steps[$stepNum].name   = (\$name)"   ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --argjson ts "$ts"     ".repos.$repoId.steps[$stepNum].ts     = (\$ts)"     ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg status "$status" ".repos.$repoId.steps[$stepNum].status = (\$status)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
  fi
}

function resetStepStatusesJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    local stepNum=0

    echo $( jq --arg origin "$(gitOrigin)" ".repos.$(gitRepoId).origin = (\$origin)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}
    echo $( jq --arg status "unknown" ".repos.$(gitRepoId).status = (\$status)" ${MICROCI_DB_JSON} ) > ${MICROCI_DB_JSON}

    yq -r .steps[].name .microCI.yml \
      | while IFS= read -r stepName
        do
          updateStepStatusJson "$(gitRepoId)" "${stepNum}" "unknown" "${stepName}"
          ((++stepNum))
        done
  fi
}

function reformatJson {
  if [ -f "${MICROCI_DB_JSON}" ]; then
    jq . ${MICROCI_DB_JSON} > /tmp/$$.json && cat /tmp/$$.json > ${MICROCI_DB_JSON}; rm -f /tmp/$$.json
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

