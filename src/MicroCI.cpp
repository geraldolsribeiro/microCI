#include <iostream>

using namespace std;

#include <spdlog/spdlog.h>

#include <MicroCI.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

namespace microci {
using nlohmann::json;

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
MicroCI::MicroCI() {
  mPluginParserMap.emplace("git_deploy", &MicroCI::parseGitDeployPluginStep);
  initBash();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::~MicroCI() {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::Script() const { return mScript.str(); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
bool MicroCI::ReadConfig(const string& filename) {
  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);
  } catch (YAML::BadFile e) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  } catch (YAML::ParserException e) {
    spdlog::error("Falha ao interpretar o arquivo .microCI.yml");
    spdlog::error(e.what());
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
    mScript << "# Imagem docker global: " << mDockerImageGlobal << endl;
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
    mScript << R"(

function main() {
  date >> .microCI.log

)";
    for (auto step : CI["steps"]) {
      string stepName = step["name"].as<string>();
      mScript << "  step_" << sanitizeName(stepName) << endl;
    }
    mScript << R"(
  date >> .microCI.log
}

# Executa todos os passos
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

  // Troca os caracteres acentuados por versão sem acento
  map<string, string> tr = {{"ç", "c"}, {"á", "a"}, {"ã", "a"}, {"ê", "e"}};
  for (auto [from, to] : tr) {
    size_t pos = 0;
    while ((pos = ret.find(from, pos)) != string::npos) {
      ret.replace(pos, from.length(), to);
      pos += to.length();
    }
  }

  // Converte tudo para minúsculas
  transform(ret.begin(), ret.end(), ret.begin(),
            [](unsigned char c) { return tolower(c); });

  // Caracteres não permitidos são trocados por _
  const auto allowedChars = "abcdefghijklmnopqrstuvwxyz01234567890";
  size_t found = ret.find_first_not_of(allowedChars);
  while (found != string::npos) {
    ret[found] = '_';
    found = ret.find_first_not_of(allowedChars, found + 1);
  }
  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseGitDeployPluginStep(YAML::Node& step) {
  auto stepName = string{};
  auto stepDescription = string{};

  if (step["description"]) {
    const auto stepName = step["name"].as<string>();
  }
  if (step["description"]) {
    stepDescription = step["description"].as<string>();
  }

  const auto name = step["plugin"]["name"].as<string>();
  const auto repo = step["plugin"]["repo"].as<string>();
  const auto gitDir = step["plugin"]["git_dir"].as<string>();
  const auto workTree = step["plugin"]["work_tree"].as<string>();

  json data;
  data["GIT_URL"] = repo;
  data["GIT_DIR"] = gitDir;
  data["GIT_WORK"] = workTree;
  data["STEP_NAME"] = stepName;
  data["FUNCTION_NAME"] = sanitizeName(stepName);
  data["STEP_DESCRIPTION"] = stepDescription;

  mScript << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  printf "${cyan}%60s${clearColor}: " "{{ STEP_NAME }}"
  {
    # Caso ainda não exista realiza o clone inicial
    if [ ! -d "{{GIT_DIR}}" ]; then
      git clone "{{GIT_URL}}" \
        --separate-git-dir="{{GIT_DIR}}" \
        "{{GIT_WORK}}"
    fi

    # Limpa a pasta -- CUIDADO AO MESCLAR REPOS
    git --git-dir="{{GIT_DIR}}" \
      --work-tree="{{GIT_WORK}}" \
      clean -xfd
    git --git-dir="{{GIT_DIR}}" \
      --work-tree="{{GIT_WORK}}" \
      checkout -f
    git --git-dir="{{GIT_DIR}}" \
      --work-tree="{{GIT_WORK}}" \
      pull

    # Remove o arquivo .git que aponta para o git-dir
    rm -f "${GIT_WORK}/.git"

    date >> .microCI.log
  }
)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parsePluginStep(YAML::Node& step) {
  auto pluginName = step["plugin"]["name"].as<string>();
  auto stepName = step["name"].as<string>();
  if (pluginName.empty() || (mPluginParserMap.count(pluginName) == 0)) {
    spdlog::error("Plugin '{}' não encontrado no passo '{}'", pluginName,
                  stepName);
    return;
  }

  parseFunctionPtr parser = mPluginParserMap[pluginName];
  (this->*parser)(step);
}

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

  auto stepName = step["name"].as<string>();

  if (step["docker"]) {
    dockerImage = step["docker"].as<string>();
  }

  if (step["description"]) {
    stepDescription = step["description"].as<string>();
  }

  data["STEP_NAME"] = stepName;
  data["STEP_DESCRIPTION"] = stepDescription;
  data["FUNCTION_NAME"] = sanitizeName(stepName);

  mScript << inja::render(R"(
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
    mScript << "      --env " << key << R"(=")" << val << R"(" \
)";
  }
  mScript << R"(      --volume "${PWD}":/ws \
    )" << dockerImage
          << R"( \
      /bin/bash -c "cd /ws)";
  for (auto cmd : cmds) {
    mScript << R"( \
         && )"
            << cmd << " 2>&1";
  }
  mScript << R"("
    status=$?
    echo "Status: ${status}"
  } 2>&1 >> .microCI.log
  if [ "${status}" = "0" ]; then
    echo -e "${green}OK${clearColor}"
  else
    echo -e "${red}FALHOU${clearColor}"
  fi
  date >> .microCI.log
}
)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::initBash() {
  nlohmann::json data;
  data["VERSION"] =
      fmt::format("{}.{}.{}    ", MAJOR, MINOR, PATCH).substr(0, 10);
  data["BLUE"] = "\033[0;34m";
  data["YELLOW"] = "\033[0;33m";
  data["MAGENTA"] = "\033[0;35m";
  data["RED"] = "\033[0;31m";
  data["GREEN"] = "\033[0;32m";
  data["CYAN"] = "\033[0;36m";
  data["CLEAR"] = "\033[0m";

  mScript << inja::render(R"(#!/bin/bash
{
  echo -e "{{BLUE}}┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓{{CLEAR}}"
  echo -e "{{BLUE}}┃                                                                    ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░█▀▀░▀█▀░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░█░█░█░░░░█░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░█▀▀░▀▀▀░▀▀▀░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░▀░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                          ░░░░░░░░░░░░░░░░░                         ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                            microCI {{ VERSION }}                       ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                           Geraldo Ribeiro                          ┃{{CLEAR}}"
  echo -e "{{BLUE}}┃                                                                    ┃{{CLEAR}}"
  echo -e "{{BLUE}}┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛{{CLEAR}}"
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

)",
                          data)
          << endl;
}

}  // namespace microci
