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
  echo -e "[0;34m┃                            microCI 0.12.0                          ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

command -v jq &> /dev/null \
  || { echo -e "[0;31mComando jq não foi encontrado[0m";  exit 1; }
command -v curl &> /dev/null \
  || { echo -e "[0;31mComando curl não foi encontrado[0m";  exit 1; }
command -v docker &> /dev/null \
  || { echo -e "[0;31mComando docker não foi encontrado[0m";  exit 1; }

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


# Notificação via Discord não será possível
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

