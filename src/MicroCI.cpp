#include <iostream>

using namespace std;

#include <spdlog/spdlog.h>

#include <MicroCI.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string banner() {
  return fmt::format(R"(
                           ░░░░░░░░░░░░░░░░░
                           ░░░░░░░█▀▀░▀█▀░░░
                           ░░░█░█░█░░░░█░░░░
                           ░░░█▀▀░▀▀▀░▀▀▀░░░
                           ░░░▀░░░░░░░░░░░░░
                           ░░░░░░░░░░░░░░░░░
                             microCI {}.{}.{}
                            Geraldo Ribeiro
)",
                     MAJOR, MINOR, PATCH);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::MicroCI() { initBash(); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::~MicroCI() {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::Bash() const { return mBash.str(); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
bool MicroCI::LoadYaml(const string& filename) {
  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);
  } catch (YAML::BadFile bf) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(bf.what());
    return false;
  }

  // Variáveis de ambiente
  if (CI["envs"] and CI["envs"].IsMap()) {
    for (auto it : CI["envs"]) {
      mEnvs.emplace(it.first.as<string>(), it.second.as<string>());
    }
  }

  // Imagem docker global (opcional)
  if (CI["docker"].IsScalar()) {
    mDockerImageGlobal = CI["docker"].as<string>();
    mBash << "# Imagem docker global: " << mDockerImageGlobal << endl;
  }

  if (CI["steps"].IsSequence()) {
    for (auto step : CI["steps"]) {
      if (step["bash"]) {
        parseBashStep(step);
      }

      if (step["plugin"]) {
        parsePluginStep(step);
      }
    }
    mBash << R"(

function main() {
  date >> .microCI.log
)";
    for (auto step : CI["steps"]) {
      string stepName = step["name"].as<string>();
      mBash << "   step_" << sanitizeName(stepName) << endl;
    }
    mBash << R"(
  date >> .microCI.log
}

main

)";
  }
  return true;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::sanitizeName(const string& name) const {
  // FIXME: tolower
  auto ret = name;
  const auto allowedChars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890";
  size_t found = ret.find_first_not_of(allowedChars);
  while (found != string::npos) {
    ret[found] = '_';
    found = ret.find_first_not_of(allowedChars, found + 1);
  }
  return ret;
}

void MicroCI::parsePluginStep(YAML::Node& step) {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseBashStep(YAML::Node& step) {
  auto ss = stringstream{step["bash"].as<string>()};
  auto cmds = vector<string>{};
  auto line = string{};
  auto stepDescription = string{};
  nlohmann::json data;
  string dockerImage = mDockerImageGlobal;

  while (getline(ss, line, '\n')) {
    cmds.push_back(line);
  }

  string stepName = step["name"].as<string>();

  if (step["docker"]) {
    dockerImage = step["docker"].as<string>();
  }

  if (step["description"]) {
    stepDescription = step["description"].as<string>();
  }

  data["STEP_NAME"] = stepName;
  data["STEP_DESCRIPTION"] = stepDescription;
  data["FUNCTION_NAME"] = sanitizeName(stepName);

  mBash << inja::render(R"(
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

  for (auto [key, val] : mEnvs) {
    mBash << "      --env " << key << R"(=")" << val << R"(" \
)";
  }
  mBash << R"(      --volume "${PWD}":/ws \
    )" << dockerImage
        << R"( \
      /bin/bash -c "cd /ws)";
  for (auto cmd : cmds) {
    mBash << R"( \
         && )"
          << cmd << " 2>&1";
  }
  mBash << R"("
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
void MicroCI::initBash() {
  nlohmann::json data;
  data["VERSION"] = fmt::format( "{}.{}.{}    ", MAJOR, MINOR, PATCH ).substr( 0, 10 );

  mBash << inja::render( R"(#!/bin/bash
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
  echo -e "${blue}┃                            microCI {{ VERSION }}                       ┃${clearColor}"
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

)", data ) << endl;
}

}  // namespace microci
