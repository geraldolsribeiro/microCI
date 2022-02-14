#!/bin/bash
{
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
  echo -e "[0;34m┃                            microCI 0.4.0                           ┃[0m"
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
  docker pull node:16 2>&1
  docker pull ubuntu:18.04 2>&1
} >> .microCI.log

# ----------------------------------------------------------------------
# Descrição do passo
# ----------------------------------------------------------------------
function step_instalar_dependencias() {
  printf "[0;36m%60s[0m: " "Instalar dependências"
  {
    (
      set -e
      echo ""
      echo ""
      echo ""
      echo "Passo: Instalar dependências"
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /ws \
        --env ENV1="xxx" \
        --env ENV2="yyy" \
        --volume "${PWD}":/ws \
        node:16 \
        /bin/bash -c "cd /ws \
           && npm install 2>&1"
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
# 
# ----------------------------------------------------------------------
function step_construir() {
  printf "[0;36m%60s[0m: " "Construir"
  {
    (
      set -e
      echo ""
      echo ""
      echo ""
      echo "Passo: Construir"
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /ws \
        --env ENV1="xxx" \
        --env ENV2="yyy" \
        --volume "${PWD}":/ws \
        node:16 \
        /bin/bash -c "cd /ws \
           && npm run lint --fix 2>&1 \
           && npm run build 2>&1"
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


function main() {
  date >> .microCI.log

  step_instalar_dependencias
  step_construir

  date >> .microCI.log
}

# Executa todos os passos
main

