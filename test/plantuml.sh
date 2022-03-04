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
  # [[ -z "$MICROCI_DISCORD_WEBHOOK" ]] && exit 0

  local result
  local content=$1
  shift

  # shellcheck disable=SC2215
  result=$( curl \
    -H "Content-Type: application/json" \
    -H "Expect: application/json" \
    --data "{\"content\": \"$content\"}" \
    -X POST "$MICROCI_DISCORD_WEBHOOK" #2>/dev/null
  )
  send_ok=$?
  [[ "${send_ok}" -ne 0 ]] && echo "fatal: curl failed with code ${send_ok}" # && exit $send_ok
  result=$(echo "${result}" | jq '.')
  echo "DISCORD: $result"
}

# :ok:
# :no_entry:
# :face_with_symbols_over_mouth:
# notify_discord ":face_with_symbols_over_mouth: Texto da notificação :pause_button: texto"

# Atualiza as imagens docker utilizadas no passos
{
  docker pull debian:stable-slim 2>&1
} >> .microCI.log

# ----------------------------------------------------------------------
# Constroi diagramas plantuml
# ----------------------------------------------------------------------
function step_gerar_diagramas_plantuml() {
  title="Gerar diagramas plantuml.............................................................."
  title=${title:0:60}
  echo -ne "[0;36m${title}[0m: "
      MICROCI_DISCORD_WEBHOOK="https://discord.com/api/webhooks/943613047375802470/sG8Esn7vuKfgqywFf2Uc30ISQLQDl__GffglPGQ5nxySNVHwApRiWL6KW8XOcw7NO-Si"

  {
    (
      set -e

      echo ""
      echo ""
      echo ""
      echo "Passo: Gerar diagramas plantuml"
      # shellcheck disable=SC2140,SC2046
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /microci_workspace \
        --env MICROCI_DISCORD_WEBHOOK="https://discord.com/api/webhooks/943613047375802470/sG8Esn7vuKfgqywFf2Uc30ISQLQDl__GffglPGQ5nxySNVHwApRiWL6KW8XOcw7NO-Si" \
        --volume "${PWD}":"/microci_workspace":rw \
        "intmain/microci_plantuml:latest" \
        /bin/bash -c "cd /microci_workspace \
          && java -jar /opt/plantuml/plantuml.jar \
          -r \
          -tsvg \
          -o docs/diagrams/ \
          src/**.cpp \
          docs/**.puml \
          2>&1"

    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "[0;32mOK[0m"
    notify_discord ":ok: $title"
  else
    echo -e "[0;31mFALHOU[0m"
    notify_discord ":face_with_symbols_over_mouth: $title"
  fi
}


# Executa todos os passos do pipeline
function main() {
  date >> .microCI.log

  step_gerar_diagramas_plantuml

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

