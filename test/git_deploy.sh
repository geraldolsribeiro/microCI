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
  echo -e "[0;34m┃                            microCI 0.11.0                          ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

# Verifica se o jq está instalado

jq --version >/dev/null 2>&1
jq_ok=$?

[[ "$jq_ok" -eq 127 ]] && \
  echo "fatal: jq not installed" && exit 2
[[ "$jq_ok" -ne 0 ]] && \
  echo "fatal: unknown error in jq" && exit 2

# Verifica se o curl está instalado
curl --version >/dev/null 2>&1
curl_ok=$?

[[ "$curl_ok" -eq 127 ]] && \
  echo "fatal: curl not installed" && exit 2

PWD=$(pwd)

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


function notify_discord() {
  {
    if [[ -z "$MICROCI_DISCORD_WEBHOOK" ]]; then
      echo "Erro: MICROCI_DISCORD_WEBHOOK não foi definida"
      return 1
    fi

    DISCORD_PAYLOAD=$( jq --null-input \
      --arg content       "$MICROCI_STEP_NAME: $MICROCI_STEP_STATUS" \
      --arg title         "$MICROCI_STEP_NAME" \
      --arg description   "$MICROCI_STEP_DESCRIPTION" \
      --arg repo          "$MICROCI_GIT_ORIGIN" \
      --arg commit        "$MICROCI_GIT_COMMIT" \
      --arg commit_msg    "$MICROCI_GIT_COMMIT_MSG" \
      --arg step_status   "$MICROCI_STEP_STATUS" \
      --arg status_color  $MICROCI_STEP_STATUS_COLOR \
      --arg step_duration "$MICROCI_STEP_DURATION" \
      '
      .content = $content                         |
      .embeds[0].title = $title                   |
      .embeds[0].color = $status_color            |
      .embeds[0].description = $description       |
      .embeds[0].fields[0].name = "Repositório"   |
      .embeds[0].fields[0].value = $repo          |
      .embeds[0].fields[0].inline = false         |
      .embeds[0].fields[1].name = "Commit"        |
      .embeds[0].fields[1].value = $commit        |
      .embeds[0].fields[1].inline = true          |
      .embeds[0].fields[2].name = "Comentário"    |
      .embeds[0].fields[2].value = $commit_msg    |
      .embeds[0].fields[2].inline = true          |
      .embeds[0].fields[3].name = "Status"        |
      .embeds[0].fields[3].value = $step_status   |
      .embeds[0].fields[3].inline = true          |
      .embeds[0].fields[4].name = "Duração"       |
      .embeds[0].fields[4].value = $step_duration |
      .embeds[0].fields[4].inline = true
      '
    )

    echo "$DISCORD_PAYLOAD"

    notify_result=$( curl \
      -H "Content-Type: application/json" \
      -H "Expect: application/json" \
      --data "$DISCORD_PAYLOAD" \
      -X POST "$MICROCI_DISCORD_WEBHOOK" 2>/dev/null
    )
    notify_status=$?

    if [[ "${notify_status}" -ne 0 ]]; then
      echo "Erro: curl falhou ao notificar discord com código ${notify_status}"
      return 1
    fi

    echo "Resultado da notificação:"
    echo "${notify_result}" | jq '.'
  } >> .microCI.log
}

# vim: ft=bash

# Atualiza as imagens docker utilizadas no passos
{
  docker pull debian:stable-slim 2>&1
} >> .microCI.log

# ----------------------------------------------------------------------
# Descrição do passo
# ----------------------------------------------------------------------
function step_deploy_de_pagina_de_aplicacao_vuejs() {
  SECONDS=0
  MICROCI_STEP_NAME="Deploy de página de aplicação VueJS"
  MICROCI_STEP_DESCRIPTION="Descrição do passo"
  MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "SEM GIT ORIGIN" )
  MICROCI_GIT_COMMIT=$( git rev-parse --short HEAD || echo "SEM GIT COMMIT")
  MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )
  MICROCI_STEP_STATUS=":ok:"
  MICROCI_STEP_DURATION=$SECONDS
  title="${MICROCI_STEP_NAME}.............................................................."
  title=${title:0:60}
  echo -ne "[0;36m${title}[0m: "
  MICROCI_DISCORD_WEBHOOK="https://discord.com/api/webhooks/943613047375802470/sG8Esn7vuKfgqywFf2Uc30ISQLQDl__GffglPGQ5nxySNVHwApRiWL6KW8XOcw7NO-Si"

  {
    (
      set -e

      # Caso ainda não exista realiza o clone inicial
      if [ ! -d "/opt/microCI/repos/app_frontend_deploy" ]; then
        git clone "git@gitlab.xyx.com.br:str/app_frontend_deploy.git" \
          --separate-git-dir="/opt/microCI/repos/app_frontend_deploy" \
          "/var/www/my-intranet/html/app" 2>&1
      fi


      # Limpa a pasta -- CUIDADO AO MESCLAR REPOS
      git --git-dir="/opt/microCI/repos/app_frontend_deploy" \
        --work-tree="/var/www/my-intranet/html/app" \
        clean -xfd 2>&1

      # Extrai a versão atual
      git --git-dir="/opt/microCI/repos/app_frontend_deploy" \
        --work-tree="/var/www/my-intranet/html/app" \
        checkout -f 2>&1 \
      && git --git-dir="/opt/microCI/repos/app_frontend_deploy" \
        --work-tree="/var/www/my-intranet/html/app" \
        pull 2>&1

      # Remove o arquivo .git que aponta para o git-dir
      rm -f "/var/www/my-intranet/html/app/.git" 2>&1

      date

    )
    status=$?
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
  } >> .microCI.log

  # Notificação no terminal
  if [ "${status}" = "0" ]; then
    echo -e "[0;32mOK[0m"
  else
    echo -e "[0;31mFALHOU[0m"
  fi

  # Notificação via Discord
  # Usar spycolor.com para obter a cor em decimal
  if [ "${status}" = "0" ]; then
    MICROCI_STEP_STATUS=":ok:"
    MICROCI_STEP_STATUS_COLOR=4382765
  else
    MICROCI_STEP_STATUS=":face_with_symbols_over_mouth:"
    MICROCI_STEP_STATUS_COLOR=16711680
  fi
  notify_discord
}


# Executa todos os passos do pipeline
function main() {
  date >> .microCI.log

  step_deploy_de_pagina_de_aplicacao_vuejs

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

