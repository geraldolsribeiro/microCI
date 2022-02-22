#!/bin/bash
{
  # Modo de conformidade com POSIX
  set -o posix

  exec 5> .microCI.dbg
  BASH_XTRACEFD="5"
  PS4='$LINENO: '

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
  echo -e "[0;34m┃                            microCI 0.6.1                           ┃[0m"
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
# Documentação usando mkdocs_material
# ----------------------------------------------------------------------
function step_cria_arquivos_iniciais_do_mkdocs() {
  title="Cria arquivos iniciais do mkdocs.............................................................."
  echo -ne "[0;36m${title:0:60}[0m: "
  {
    (
      set -e

      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /microci_workspace \
        --volume "${PWD}":/microci_workspace \
        --publish 8000:8000 \
        squidfunk/mkdocs-material \
        new . 2>&1

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
  {
    (
      set -e

      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /microci_workspace \
        --volume "${PWD}":/microci_workspace \
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

# ----------------------------------------------------------------------
# Publica arquivos em um repositório git
# ----------------------------------------------------------------------
function step_publicar_html_para_repositorio_git() {
  title="Publicar HTML para repositório git.............................................................."
  echo -ne "[0;36m${title:0:60}[0m: "
  {
    (
      set -e

      echo ""
      echo ""
      echo ""
      echo "Passo: Publicar HTML para repositório git"
      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /microci_workspace \
        --volume "${HOME}/.ssh":"/.microCI_ssh":ro \
        --volume "${PWD}":"/microci_workspace":rw \
        "bitnami/git:latest" \
        /bin/bash -c "cd /microci_workspace \
           && cp -Rv /.microCI_ssh /root/.ssh 2>&1 \
           && chmod 700 /root/.ssh/ 2>&1 \
           && chmod 644 /root/.ssh/id_rsa.pub 2>&1 \
           && chmod 600 /root/.ssh/id_rsa 2>&1 \
           && git clone 'ssh://git@someurl.com/site.git' --depth 1 '/deploy' 2>&1 \
           && git -C /deploy config user.name  '$(git config --get user.name)' 2>&1 \
           && git -C /deploy config user.email '$(git config --get user.email)' 2>&1 \
           && git -C /deploy rm '*' 2>&1 \
           && cp -rv site/* /deploy/ 2>&1 \
           && git -C /deploy add . 2>&1 \
           && git -C /deploy commit -am ':rocket:microCI git_publish' 2>&1 \
           && git -C /deploy push origin master 2>&1 \
           && chown $(id -u):$(id -g) -Rv site 2>&1
  "
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
  step_publicar_html_para_repositorio_git

  date >> .microCI.log
}

main

# Para executar use
# microCI | bash

