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


