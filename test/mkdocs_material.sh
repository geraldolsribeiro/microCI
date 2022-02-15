#!/bin/bash
{
  # Modo de conformidade com POSIX
  set -o posix

  exec 5> .microCI.dbg
  BASH_XTRACEFD="5"
  PS4='$LINENO: '

  echo -e "[0;34m┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░█▀▀░▀█▀░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░█░█░█░░░░█░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░█▀▀░▀▀▀░▀▀▀░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░▀░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                          ░░░░░░░░░░░░░░░░░                         ┃[0m"
  echo -e "[0;34m┃                            microCI 0.5.0                           ┃[0m"
  echo -e "[0;34m┃                           Geraldo Ribeiro                          ┃[0m"
  echo -e "[0;34m┃                                                                    ┃[0m"
  echo -e "[0;34m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛[0m"
  echo ""
  echo ""
} | tee .microCI.log

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


# Atualiza as imagens docker utilizadas no passos
{
  docker pull debian:stable-slim 2>&1
} >> .microCI.log

# ----------------------------------------------------------------------
# 
# ----------------------------------------------------------------------
function step_cria_arquivos_iniciais_do_mkdocs() {
  title="Cria arquivos iniciais do mkdocs.............................................................."
  echo -ne "[0;36m${title:0:60}[0m: "
  # printf "[0;36m%60s[0m: " "Cria arquivos iniciais do mkdocs"
  {
    (
      set -e

      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /ws \
        --volume "${PWD}":/ws \
        --publish 8000:8000 \
        squidfunk/mkdocs-material \
        init 2>&1

    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "[0;32mOK[0m"
  else
    echo -e "[0;31mFALHOU[0m"
  fi
}

# ----------------------------------------------------------------------
# Documentação do projeto
# ----------------------------------------------------------------------
function step_construir_documentacao_em_formato_html() {
  title="Construir documentação em formato HTML.............................................................."
  echo -ne "[0;36m${title:0:60}[0m: "
  # printf "[0;36m%60s[0m: " "Construir documentação em formato HTML"
  {
    (
      set -e

      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /ws \
        --volume "${PWD}":/ws \
        --publish 8000:8000 \
        squidfunk/mkdocs-material \
        build 2>&1

    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "[0;32mOK[0m"
  else
    echo -e "[0;31mFALHOU[0m"
  fi
}


# Executa todos os passos do pipeline
function main() {
  date >> .microCI.log

  step_cria_arquivos_iniciais_do_mkdocs
  step_construir_documentacao_em_formato_html

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

