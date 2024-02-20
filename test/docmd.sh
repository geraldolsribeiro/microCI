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
  echo -e "[0;34m┃                            microCI v0.29.0                         ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

# Verifica se as dependências estão presentes
command -v jq &> /dev/null \
  || { echo -e "[0;31mComando jq não foi encontrado[0m"; exit 1; }

command -v yq &> /dev/null \
  || { echo -e "[0;31mComando yq não foi encontrado[0m"; exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mComando curl não foi encontrado[0m"; exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mComando docker não foi encontrado[0m"; exit 1; }

# Caminho físico sem quebra de linha
MICROCI_PWD=$(pwd -P | tr -d '\n')
MICROCI_DB_JSON=/opt/microCI/db.json
MICROCI_STEP_NUMBER=0

function gitOrigin {
  git config --get remote.origin.url || echo "SEM GIT ORIGIN"
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


# Notificação via Discord não será possível

# ----------------------------------------------------------------------
# Extract documentation from source code to a markdown file
# ----------------------------------------------------------------------
function step_extract_documentation() {
  SECONDS=0
  MICROCI_STEP_NAME="Extract documentation"
  MICROCI_STEP_DESCRIPTION="Extract documentation from source code to a markdown file"
  MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "SEM GIT ORIGIN" )
  MICROCI_GIT_COMMIT_SHA=$( git rev-parse --short HEAD || echo "SEM GIT COMMIT")
  MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )
  MICROCI_STEP_STATUS=":ok:"
  MICROCI_STEP_SKIP="no"
  MICROCI_STEP_DURATION=$SECONDS
  title="$(( MICROCI_STEP_NUMBER + 1 )) ${MICROCI_STEP_NAME}.............................................................."
  title=${title:0:60}
  echo -ne "[0;36m${title}[0m: "
  INTMAIN_DOCMD_DETAILS="false"
  INTMAIN_DOCMD_SHOW_BANNER="true"
  INTMAIN_DOCMD_SHOW_SOURCE="false"
  INTMAIN_DOCMD_TOC="false"

  {
    (
      set -e

      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/bash.yml docs/plugin_bash.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/beamer.yml docs/plugin_beamer.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/clang-format_config.yml docs/plugin_clang-format_config.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/clang-format.yml docs/plugin_clang-format.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/clang-tidy.yml docs/plugin_clang-tidy.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/cppcheck.yml docs/plugin_cppcheck.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/fetch2.yml docs/plugin_fetch2.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/fetch.yml docs/plugin_fetch.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/git_deploy.yml docs/plugin_git_deploy.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/git_publish.yml docs/plugin_git_publish.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/minio.yml docs/plugin_minio.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/mkdocs_material_config.yml docs/plugin_mkdocs_material_config.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/mkdocs_material.yml docs/plugin_mkdocs_material.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/npm.yml docs/plugin_npm.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/pandoc.yml docs/plugin_pandoc.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/plantuml.yml docs/plugin_plantuml.md
      echo ""
      echo ""
      echo ""
      echo "Passo: Extract documentation"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --env INTMAIN_DOCMD_DETAILS="false" \
        --env INTMAIN_DOCMD_SHOW_BANNER="true" \
        --env INTMAIN_DOCMD_SHOW_SOURCE="false" \
        --env INTMAIN_DOCMD_TOC="false" \
        --volume "${MICROCI_PWD}":"/microci_workspace":rw \
        "intmain/microci_docmd:0.2" yaml new/skip.yml docs/plugin_skip.md
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
    echo "Veja o log completo em .microCI.log"
    tail -50 .microCI.log
  fi

  ((++MICROCI_STEP_NUMBER))
}
# Atualiza as imagens docker utilizadas no passos
echo 'Atualizando imagem docker debian:stable-slim...'
if docker image inspect debian:stable-slim > /dev/null 2>&1 ; then  echo 'Imagem docker debian:stable-slim está atualizada' >> .microCI.log
else
  docker pull debian:stable-slim 2>&1 >> .microCI.log
fi
echo 'Atualizando imagem docker intmain/microci_docmd:0.2...'
if docker image inspect intmain/microci_docmd:0.2 > /dev/null 2>&1 ; then  echo 'Imagem docker intmain/microci_docmd:0.2 está atualizada' >> .microCI.log
else
  docker pull intmain/microci_docmd:0.2 2>&1 >> .microCI.log
fi


# Executa todos os passos do pipeline
function main() {
  date >> .microCI.log

  step_extract_documentation

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

