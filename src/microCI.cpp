// TODO: saída para relatório
// https://github.com/p-ranav/tabulate
// https://docs.docker.com/engine/reference/commandline/run/
// https://plantuml.com/yaml

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <argh.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

// using namespace inja;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string banner() {
  return R"(
                           ░░░░░░░░░░░░░░░░░
                           ░░░░░░░█▀▀░▀█▀░░░
                           ░░░█░█░█░░░░█░░░░
                           ░░░█▀▀░▀▀▀░▀▀▀░░░
                           ░░░▀░░░░░░░░░░░░░
                           ░░░░░░░░░░░░░░░░░
                             microCI 0.0.1
                            Geraldo Ribeiro
)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opções:
  -h --help                Ajuda
  -i,--input arquivo.yml   Carrega arquivo de configuração

)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string sanitizeName(const string &s) {
  // FIXME: tolower
  auto ret = s;
  const auto allowedChars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890";
  size_t found = ret.find_first_not_of(allowedChars);
  while (found != string::npos) {
    ret[found] = '_';
    found = ret.find_first_not_of(allowedChars, found + 1);
  }
  return ret;
}

void parsePluginStep() {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void parseBashStep(const map<string, string> &envs, const string &dockerImage,
                   const string &stepName, const string &stepDescription,
                   const string s) {
  auto ss = stringstream{s};
  auto cmds = vector<string>{};
  auto line = string{};
  nlohmann::json data;

  while (getline(ss, line, '\n')) {
    cmds.push_back(line);
  }

  data["STEP_NAME"] = stepName;
  data["STEP_DESCRIPTION"] = stepDescription;
  data["FUNCTION_NAME"] = sanitizeName(stepName);

  cout << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  printf "${cyan}%60s${clearColor}: " "{{ STEP_NAME }}"
  {
    echo ""
    echo ""
    echo ""
    echo "Passo: {{ STEP_NAME }}"
    docker run \
      --interactive \
      --attach stdout \
      --attach stderr \
      --rm \
      --workdir /ws \
)",
                       data);

  for (auto [key, val] : envs) {
    cout << "      --env " << key << R"(=")" << val << R"(" \
)";
  }
  cout << R"(      --volume "${PWD}":/ws \
    )" << dockerImage
       << R"( \
      /bin/bash -c "cd /ws)";
  for (auto cmd : cmds) {
    cout << R"( \
         && )"
         << cmd << " 2>&1";
  }
  cout << R"("
    status=$?
    echo "Status: ${status}"
  } 2>&1 >> .microCI.log
  if [ "${status}" = "0" ]; then
    echo -e "${green}OK${clearColor}"
  else
    echo -e "${red}FALHOU${clearColor}"
  fi
}
)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  //{{{
  argh::parser cmdl(
      argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);  // -i arq
  YAML::Node CI;
  string yamlFilename = ".microCI.yml";

  if (cmdl[{"-h", "--help"}]) {
    cout << banner() << help() << endl;
    return 0;
  }

  if (!(cmdl({"-i", "--input"}) >> yamlFilename)) {
    spdlog::debug("Usando arquivo de entrada padrão {}", yamlFilename);
  }

  if (!filesystem::exists(yamlFilename)) {
    spdlog::error("Arquivo de entrada {} não encontrado.", yamlFilename);
    return 1;
  }

  try {
    CI = YAML::LoadFile(yamlFilename);
  } catch (YAML::BadFile bf) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(bf.what());
    return 1;
  }

  string dockerImageGlobal;
  map<string, string> envs;

  cout << R"(#!/bin/bash
red='\033[0;31m'
green='\033[0;32m'
yellow='\033[0;33m'
blue='\033[0;34m'
magenta='\033[0;35m'
cyan='\033[0;36m'
clearColor='\033[0m'

{
  echo -e "${blue}┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓${clearColor}"
  echo -e "${blue}┃                                                                    ┃${clearColor}"
  echo -e "${blue}┃                          ░░░░░░░░░░░░░░░░░                         ┃${clearColor}"
  echo -e "${blue}┃                          ░░░░░░░█▀▀░▀█▀░░░                         ┃${clearColor}"
  echo -e "${blue}┃                          ░░░█░█░█░░░░█░░░░                         ┃${clearColor}"
  echo -e "${blue}┃                          ░░░█▀▀░▀▀▀░▀▀▀░░░                         ┃${clearColor}"
  echo -e "${blue}┃                          ░░░▀░░░░░░░░░░░░░                         ┃${clearColor}"
  echo -e "${blue}┃                          ░░░░░░░░░░░░░░░░░                         ┃${clearColor}"
  echo -e "${blue}┃                            microCI 0.0.1                           ┃${clearColor}"
  echo -e "${blue}┃                           Geraldo Ribeiro                          ┃${clearColor}"
  echo -e "${blue}┃                                                                    ┃${clearColor}"
  echo -e "${blue}┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛${clearColor}"
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

)" << endl;

  // Variáveis de ambiente
  if (CI["envs"] and CI["envs"].IsMap()) {
    for (auto it : CI["envs"]) {
      envs.emplace(it.first.as<string>(), it.second.as<string>());
    }
  }

  // Imagem docker global (opcional)
  if (CI["docker"].IsScalar()) {
    dockerImageGlobal = CI["docker"].as<string>();
    cout << "# Imagem docker global: " << dockerImageGlobal << endl;
  }

  if (CI["steps"].IsSequence()) {
    for (auto step : CI["steps"]) {
      string dockerImage = dockerImageGlobal;
      string stepName = step["name"].as<string>();
      string stepDescription;

      if (step["docker"]) {
        dockerImage = step["docker"].as<string>();
      }

      if (step["description"]) {
        stepDescription = step["description"].as<string>();
      }

      if (step["bash"]) {
        parseBashStep(envs, dockerImage, stepName, stepDescription,
                      step["bash"].as<string>());
      }

      if (step["plugin"]) {
        parsePluginStep();
      }
    }
    cout << R"(

function main() {
  date >> .microCI.log
)";
    for (auto step : CI["steps"]) {
      string stepName = step["name"].as<string>();
      cout << "   step_" << sanitizeName(stepName) << endl;
    }
    cout << R"(
  date >> .microCI.log
}

main

)";
  }
  return 0;
}

