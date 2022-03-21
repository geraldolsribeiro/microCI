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
  echo -e "[0;34m┃                            microCI 0.16.0                          ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

# Verifica se as dependências estão presentes
command -v jq &> /dev/null \
  || { echo -e "[0;31mComando jq não foi encontrado[0m";  exit 1; }

command -v curl &> /dev/null \
  || { echo -e "[0;31mComando curl não foi encontrado[0m";  exit 1; }

command -v docker &> /dev/null \
  || { echo -e "[0;31mComando docker não foi encontrado[0m";  exit 1; }

PWD=$(pwd)

function microCI_latest_download_URL_with_version {
  curl -s https://api.github.com/repos/geraldolsribeiro/microci/releases/latest \
    | grep browser_download_url \
    | grep -o -E "https://github.com/geraldolsribeiro/microCI/releases/download/(.*)/microCI"
}

function microCI_download_latest_binary {
  curl -fsSL github.com/geraldolsribeiro/microci/releases/latest/download/microCI -o /usr/local/bin/microCI
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


# Notificação via Discord não será possível
# Atualiza as imagens docker utilizadas no passos
{
  docker pull debian:stable-slim 2>&1
} >> .microCI.log

# ----------------------------------------------------------------------
# Verifica o código C++ e gera relatório em formato HTML
# ----------------------------------------------------------------------
function step_gerar_relatorio_de_verificacao_do_codigo_c_____clang_tidy() {
  SECONDS=0
  MICROCI_STEP_NAME="Gerar relatório de verificação do código C++ - clang-tidy"
  MICROCI_STEP_DESCRIPTION="Verifica o código C++ e gera relatório em formato HTML"
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

      echo ""
      echo ""
      echo ""
      echo "Passo: Gerar relatório de verificação do código C++ - clang-tidy"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network none \
        --workdir /microci_workspace \
        --volume "${PWD}":"/microci_workspace":rw \
        "intmain/microci_cppcheck:latest" \
        /bin/bash -c "cd /microci_workspace \
        && mkdir -p auditing/clang-tidy/ \
        && date > auditing/clang-tidy/clang-tidy.log \
        && clang-tidy \
        src/*.cpp \
        test/*.cpp \
        -checks='*' \
        -- \
        -std=c++11 \
        -I/usr/include/ \
        2>&1 | tee auditing/clang-tidy/clang-tidy.log 2>&1 \
       && clang-tidy-html auditing/clang-tidy/clang-tidy.log 2>&1 \
       && mv -v clang-tidy-checks.py auditing/clang-tidy/ 2>&1 \
       && mv -v clang.html auditing/clang-tidy/index.html 2>&1 \
       && chown $(id -u):$(id -g) -Rv auditing 2>&1"

    )

    status=$?
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
    echo "Duration: ${MICROCI_STEP_DURATION}"
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

  step_gerar_relatorio_de_verificacao_do_codigo_c_____clang_tidy

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

