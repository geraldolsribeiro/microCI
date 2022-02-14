// ░░░░░░░░░░░░░░░░░ microCI
// ░░░░░░░█▀▀░▀█▀░░░
// ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
// ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// ░░░▀░░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░ Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <iostream>

using namespace std;

#include <spdlog/spdlog.h>

#include <MicroCI.hpp>
#include <inja.hpp>

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
MicroCI::MicroCI() {
  mPluginParserMap.emplace("git_deploy", &MicroCI::parseGitDeployPluginStep);
  mPluginParserMap.emplace("mkdocs_material",
                           &MicroCI::parseMkdocsMaterialPluginStep);
  mDockerImageGlobal = "debian:stable-slim";
  initBash();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::~MicroCI() {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetOnlyStep(const string& onlyStep) { mOnlyStep = onlyStep; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::Script() const { return mScript.str(); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
bool MicroCI::ReadConfig(const string& filename) {
  YAML::Node CI;
  set<string> dockerImages;

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

  // Localiza imagens docker necessárias
  for (auto step : CI["steps"]) {
    if (step["docker"]) {
      dockerImages.insert(step["docker"].as<string>());
    }
  }

  // Imagem docker global (opcional)
  if (CI["docker"].IsScalar()) {
    mDockerImageGlobal = CI["docker"].as<string>();
    dockerImages.insert(mDockerImageGlobal);
  }

  mScript << "# Atualiza as imagens docker utilizadas no passos\n{\n";
  for (const auto& dockerImage : dockerImages) {
    mScript << fmt::format("  docker pull {} 2>&1\n", dockerImage);
  }
  mScript << "} >> .microCI.log\n";

  if (!mOnlyStep.empty()) {
    for (auto step : CI["steps"]) {
      if (step["only"] and step["only"].as<string>() == mOnlyStep) {
        if (step["bash"]) {
          parseBashStep(step);
        } else if (step["plugin"]) {
          parsePluginStep(step);
        }
        mScript << "# Executa somente este passo" << endl;
        mScript << "step_" << sanitizeName(step["name"].as<string>()) << endl;
        mScript << "exit 0;" << endl;
      }
    }
  } else {
    if (CI["steps"].IsSequence()) {
      for (auto step : CI["steps"]) {
        if (step["only"]) {
          continue;
        } else if (step["bash"]) {
          parseBashStep(step);
        } else if (step["plugin"]) {
          parsePluginStep(step);
        }
      }
      mScript << R"(

function main() {
  date >> .microCI.log

)";
      for (auto step : CI["steps"]) {
        if (step["only"]) {
          continue;
        }
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
void MicroCI::parseMkdocsMaterialPluginStep(YAML::Node& step) {
  auto stepName = string{};
  auto stepDescription = string{};
  auto action = string{"build"};
  auto port = string{"8000"};

  if (step["name"]) {
    stepName = step["name"].as<string>();
  }
  if (step["description"]) {
    stepDescription = step["description"].as<string>();
  }
  if (step["plugin"]["action"]) {
    action = step["plugin"]["action"].as<string>();
    if (action == "serve") {
      action += " --dev-addr=0.0.0.0:8000";
    }
  }
  if (step["plugin"]["port"]) {
    port = step["plugin"]["port"].as<string>();
  }

  auto data = defaultDataTemplate();
  data["ACTION"] = action;
  data["PORT"] = port;
  data["STEP_NAME"] = stepName;
  data["FUNCTION_NAME"] = sanitizeName(stepName);
  data["STEP_DESCRIPTION"] = stepDescription;

  // https://unix.stackexchange.com/questions/155551/how-to-debug-a-bash-script
  // exec 5> >(logger -t $0)
  // BASH_XTRACEFD="5"
  // PS4='$LINENO: '
  // set -x
  //
  // # Place your code here

  mScript << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  printf "{{CYAN}}%60s{{CLEAR}}: " "{{ STEP_NAME }}"
  {
    (
      set -e
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir /docs \
        --volume "${PWD}":/docs \
        --publish {{PORT}}:8000 \
        squidfunk/mkdocs-material \
        {{ACTION}} 2>&1
    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "{{GREEN}}OK{{CLEAR}}"
  else
    echo -e "{{RED}}FALHOU{{CLEAR}}"
  fi
}
)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseGitDeployPluginStep(YAML::Node& step) {
  auto stepName = string{};
  auto stepDescription = string{};

  if (step["name"]) {
    stepName = step["name"].as<string>();
  }
  if (step["description"]) {
    stepDescription = step["description"].as<string>();
  }

  const auto name = step["plugin"]["name"].as<string>();
  const auto repo = step["plugin"]["repo"].as<string>();
  const auto gitDir = step["plugin"]["git_dir"].as<string>();
  const auto workTree = step["plugin"]["work_tree"].as<string>();

  auto data = defaultDataTemplate();
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
  printf "{{CYAN}}%60s{{CLEAR}}: " "{{ STEP_NAME }}"
  {
    (
      set -e
      # Caso ainda não exista realiza o clone inicial
      if [ ! -d "{{GIT_DIR}}" ]; then
        git clone "{{GIT_URL}}" \
          --separate-git-dir="{{GIT_DIR}}" \
          "{{GIT_WORK}}" 2>&1
      fi

      # Limpa a pasta -- CUIDADO AO MESCLAR REPOS
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        clean -xfd 2>&1 \
      && git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        checkout -f 2>&1 \
      && git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        pull 2>&1

      # Remove o arquivo .git que aponta para o git-dir
      rm -f "{{GIT_WORK}}/.git" 2>&1

      date
    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "{{GREEN}}OK{{CLEAR}}"
  else
    echo -e "{{RED}}FALHOU{{CLEAR}}"
  fi
}
)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parsePluginStep(YAML::Node& step) {
  auto pluginName = step["plugin"]["name"].as<string>();
  if (pluginName.empty() || (mPluginParserMap.count(pluginName) == 0)) {
    auto stepName = step["name"].as<string>();
    spdlog::error("Plugin '{}' não encontrado no passo '{}'", pluginName,
                  stepName);
    return;
  }

  // Executa o plugin
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
  auto data = defaultDataTemplate();
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
  printf "{{CYAN}}%60s{{CLEAR}}: " "{{ STEP_NAME }}"
  {
    (
      set -e
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
    mScript << "        --env " << key << R"(=")" << val << R"(" \
)";
  }
  mScript << R"(        --volume "${PWD}":/ws \
        )" << dockerImage
          << R"( \
        /bin/bash -c "cd /ws)";
  for (auto cmd : cmds) {
    mScript << R"( \
           && )"
            << cmd << " 2>&1";
  }
  mScript << inja::render(R"("
    )
    status=$?
    echo "Status: ${status}"
  } >> .microCI.log

  if [ "${status}" = "0" ]; then
    echo -e "{{GREEN}}OK{{CLEAR}}"
  else
    echo -e "{{RED}}FALHOU{{CLEAR}}"
  fi
}
)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
json MicroCI::defaultDataTemplate() const {
  json data;
  data["VERSION"] =
      fmt::format("{}.{}.{}    ", MAJOR, MINOR, PATCH).substr(0, 10);
  data["BLUE"] = "\033[0;34m";
  data["YELLOW"] = "\033[0;33m";
  data["MAGENTA"] = "\033[0;35m";
  data["RED"] = "\033[0;31m";
  data["GREEN"] = "\033[0;32m";
  data["CYAN"] = "\033[0;36m";
  data["CLEAR"] = "\033[0m";
  return data;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::initBash() {
  auto data = defaultDataTemplate();

  mScript << inja::render(R"(#!/bin/bash
{
  exec 5> .microCI.dbg
  BASH_XTRACEFD="5"
  PS4='$LINENO: '

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

)",
                          data)
          << endl;
}

}  // namespace microci
