// MIT License
//
// Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
//
// ░░░░░░░░░░░░░░░░░
// ░░░░░░░█▀▀░▀█▀░░░
// ░░░█░█░█░░░░█░░░░
// ░░░█▀▀░▀▀▀░▀▀▀░░░
// ░░░▀░░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

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
  mPluginParserMap.emplace("git_publish", &MicroCI::parseGitPublishPluginStep);
  mPluginParserMap.emplace("mkdocs_material",
                           &MicroCI::parseMkdocsMaterialPluginStep);
  mPluginParserMap.emplace("cppcheck", &MicroCI::parseCppCheckPluginStep);

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
  } catch (const YAML::BadFile e) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  } catch (const YAML::ParserException e) {
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
  }

  if (!mDockerImageGlobal.empty()) {
    dockerImages.insert(mDockerImageGlobal);
  }

  if (dockerImages.size()) {
    mScript << "# Atualiza as imagens docker utilizadas no passos\n{\n";
    for (const auto& dockerImage : dockerImages) {
      mScript << fmt::format("  docker pull {} 2>&1\n", dockerImage);
    }
    mScript << "} >> .microCI.log\n";
  }

  if (!mOnlyStep.empty()) {
    for (auto step : CI["steps"]) {
      if (step["only"] and step["only"].as<string>() == mOnlyStep) {
        if (step["bash"] || step["sh"]) {
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
        } else if (step["bash"] || step["sh"]) {
          parseBashStep(step);
        } else if (step["plugin"]) {
          parsePluginStep(step);
        }
      }
      mScript << R"(

# Executa todos os passos do pipeline
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

main

# Para executar use
# microCI | bash

)";
    }
  }
  return true;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::sanitizeName(const string& name) const {
  auto ret = name;

  // Troca os caracteres acentuados por versão sem acento
  map<string, string> tr = {{"ç", "c"}, {"á", "a"}, {"ã", "a"},
                            {"ê", "e"}, {"ó", "o"}, {"õ", "o"}};
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
void MicroCI::parsePluginStep(const YAML::Node& step) {
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
void MicroCI::beginFunction(const json& data) {
  mScript << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  title="{{ STEP_NAME }}.............................................................."
  echo -ne "{{CYAN}}${title:0:60}{{CLEAR}}: "
  {
    (
      set -e
)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::endFunction(const json& data) {
  mScript << inja::render(R"(
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
void MicroCI::prepareRunDocker(const json& data,
                               const set<DockerVolume>& volumes) {
  mScript << inja::render(R"(
      echo ""
      echo ""
      echo ""
      echo "Passo: {{ STEP_NAME }}"
      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir {{ WORKSPACE }} \
)",
                          data);

  for (auto [key, val] : mEnvs) {
    mScript << fmt::format("        --env {}=\"{}\" \\\n", key, val);
  }

  for (const auto& vol : volumes) {
    mScript << fmt::format("        --volume \"{}\":\"{}\":{} \\\n", vol.source,
                           vol.destination, vol.mode);
  }

  mScript << inja::render("        \"{{ DOCKER_IMAGE }}\" \\\n", data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseMkdocsMaterialPluginStep(const YAML::Node& step) {
  auto action = string{"build"};
  auto port = string{"8000"};

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
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Documentação usando mkdocs_material");
  data["DOCKER_IMAGE"] = "squidfunk/mkdocs-material";
  // data["DOCKER_IMAGE"] = "microci_mkdocs_material";

  // https://unix.stackexchange.com/questions/155551/how-to-debug-a-bash-script
  // exec 5> >(logger -t $0)
  // BASH_XTRACEFD="5"
  // PS4='$LINENO: '
  // set -x
  //
  // # Place your code here

  beginFunction(data);

  mScript << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir {{ WORKSPACE }} \
        --volume "${PWD}":{{ WORKSPACE }} \
        --publish {{PORT}}:8000 \
        {{ DOCKER_IMAGE }} \
        {{ ACTION }} 2>&1
)",
                          data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseCppCheckPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto platform = string{"unix64"};
  auto standard = string{"c++11"};
  list<string> includeList;
  list<string> sourceList;
  list<string> opts{"--enable=all", "--inconclusive", "--xml",
                    "--xml-version=2"};

  // opções disponíveis:
  // cppcheck [--check-config] [--check-library] [-D<id>] [-U<id>]
  // [--enable=<id>] [--error-exitcode=<n>] [--errorlist]
  // [--exitcode-suppressions=<file>]
  // [--file-list=<file>] [--force]
  // [--includes-file=<file>] [--config-exclude=<dir>]
  // [--config-excludes-file=<file>] [--include=<file>]
  // [-i<dir>] [--inline-suppr] [-j<jobs>]
  // [-l<load>] [--language=<language>] [--library=<cfg>]
  // [--max-configs=<limit>] [--max-ctu-depth=<limit>]
  // [--platform=<type>] [--quiet]
  // [--relative-paths=<paths>] [--report-progress]
  // [--rule=<rule>] [--rule-file=<file>]
  // [--suppress=<spec>]
  // [--suppressions-list=<file>]
  // [--suppress-xml=<.xml file>]
  // [--template='<text>'] [--verbose]

  if (step["plugin"]["include"] && step["plugin"]["include"].IsSequence()) {
    for (const auto& inc : step["plugin"]["include"]) {
      includeList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["src"] && step["plugin"]["src"].IsSequence()) {
    for (const auto& src : step["plugin"]["src"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  if (step["plugin"]["platform"]) {
    platform = step["plugin"]["platform"].as<string>();
  }

  if (step["plugin"]["std"]) {
    standard = step["plugin"]["std"].as<string>();
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "intmain/microci_cppcheck:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Verifica código C++");
  data["PLATFORM"] = platform;
  data["STD"] = standard;
  data["REPORT_TITLE"] = "MicroCI::CppCheck";

  beginFunction(data);
  prepareRunDocker(data, volumes);

  mScript << inja::render(R"(        /bin/bash -c "cd {{ WORKSPACE }} \
      && mkdir -p auditing/cppcheck \
      && cppcheck \
        --platform={{ PLATFORM }} \
        --std={{ STD }} \
)", data);

  for (const auto& opt : opts) {
    mScript << "        " << opt << " \\\n";
  }

  for (const auto& inc : includeList) {
    mScript << "        --include=" << inc << " \\\n";
  }

  for (const auto& src : sourceList) {
    mScript << "        " << src << " \\\n";
  }

  // xml é escrito na saída de erro
  mScript << inja::render( R"(        2> auditing/cppcheck.xml \
      && cppcheck-htmlreport \
        --title='{{ REPORT_TITLE }}' \
        --report-dir='auditing/cppcheck/' \
        --source-dir='.' \
        --file='auditing/cppcheck.xml' 2>&1 \
      && chown $(id -u):$(id -g) -Rv auditing 2>&1"
)", data );

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseGitPublishPluginStep(const YAML::Node& step) {
  auto copyTo = string{"/microci_deploy"};
  auto copyFrom = string{"site"};
  auto cleanBefore = true;
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  tie(data, volumes) = parseSsh(step, data, volumes);

  const auto name = step["plugin"]["name"].as<string>();
  const auto gitURL = step["plugin"]["git_url"].as<string>();

  if (step["plugin"]["copy_from"]) {
    copyFrom = step["plugin"]["copy_from"].as<string>();
  }

  if (step["plugin"]["copy_to"]) {
    copyTo = step["plugin"]["copy_to"].as<string>();
  }

  if (step["plugin"]["clean_before"]) {
    cleanBefore = step["plugin"]["clean_before"].as<bool>();
  }

  data["GIT_URL"] = gitURL;
  data["COPY_TO"] = copyTo;
  data["COPY_FROM"] = copyFrom;
  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "bitnami/git:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Publica arquivos em um repositório git");

  beginFunction(data);
  prepareRunDocker(data, volumes);

  mScript << inja::render("        /bin/bash -c \"cd {{ WORKSPACE }}", data);

  if (step["ssh"]) {
    copySsh(step, data);
  }

  mScript << inja::render(R"( \
           && git clone '{{ GIT_URL }}' --depth 1 '{{ COPY_TO }}' 2>&1 \
           && git -C {{ COPY_TO }} config user.name  '$(git config --get user.name)' 2>&1 \
           && git -C {{ COPY_TO }} config user.email '$(git config --get user.email)' 2>&1 \)",
                          data);

  if (cleanBefore) {
    mScript << inja::render(R"(
           && git -C {{ COPY_TO }} rm '*' 2>&1 \)",
                            data);
  }

  mScript << inja::render(R"(
           && cp -rv {{ COPY_FROM }}/* {{ COPY_TO }}/ 2>&1 \
           && git -C {{ COPY_TO }} add . 2>&1 \
           && git -C {{ COPY_TO }} commit -am ':rocket:microCI git_publish' 2>&1 \
           && git -C {{ COPY_TO }} push origin master 2>&1 \
           && chown $(id -u):$(id -g) -Rv {{ COPY_FROM }} 2>&1
  ")",
                          data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseGitDeployPluginStep(const YAML::Node& step) {
  const auto name = step["plugin"]["name"].as<string>();
  const auto repo = step["plugin"]["repo"].as<string>();
  const auto gitDir = step["plugin"]["git_dir"].as<string>();
  const auto workTree = step["plugin"]["work_tree"].as<string>();

  auto data = defaultDataTemplate();
  data["GIT_URL"] = repo;
  data["GIT_DIR"] = gitDir;
  data["GIT_WORK"] = workTree;
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step);

  beginFunction(data);
  mScript << inja::render(R"(
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
)",
                          data);
  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
set<DockerVolume> MicroCI::parseVolumes(const YAML::Node& step) const {
  auto volumes = defaultVolumes();

  if (step["volumes"] && step["volumes"].IsSequence()) {
    for (const auto& volume : step["volumes"]) {
      DockerVolume vol;
      vol.destination = volume["destination"].as<string>();
      vol.source = volume["source"].as<string>();
      if (volume["mode"]) {
        vol.mode = volume["mode"].as<string>();
      } else {
        vol.mode = "ro";
      }
      volumes.insert(vol);
    }
  }

  return volumes;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::stepRequiredValue(const YAML::Node& step,
                                  const string& var) const {
  if (!step[var]) {
    throw std::invalid_argument(
        fmt::format("Campo {} não encontrado no passo", var));
  }
  return step[var].as<string>();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::stepOptionalValue(const YAML::Node& step, const string& var,
                                  const string& defaultValue) const {
  if (step[var]) {
    return step[var].as<string>();
  } else {
    return defaultValue;
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::stepName(const YAML::Node& step) const {
  return stepRequiredValue(step, "name");
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::stepDescription(const YAML::Node& step,
                                const string& defaultValue) const {
  return stepOptionalValue(step, "description", defaultValue);
  return defaultValue;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::stepDockerImage(const YAML::Node& step,
                                const string& image) const {
  string dockerImage = mDockerImageGlobal;

  if (!image.empty()) {
    dockerImage = image;
  }

  if (step["docker"]) {
    dockerImage = step["docker"].as<string>();
  }

  return dockerImage;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
tuple<json, set<DockerVolume>> MicroCI::parseSsh(
    const YAML::Node& step, const json& data,
    const set<DockerVolume>& volumes) const {
  auto sshCopyFrom = string{};
  auto sshCopyTo = string{};

  auto volumes_ = volumes;
  auto data_ = data;

  if (step["ssh"]) {
    sshCopyFrom = "${HOME}/.ssh";
    sshCopyTo = "/root/.ssh";

    if (step["ssh"]["copy_from"]) {
      sshCopyFrom = step["ssh"]["copy_from"].as<string>();
    }

    if (step["ssh"]["copy_to"]) {
      sshCopyTo = step["ssh"]["copy_to"].as<string>();
    }

    // Montagem temporária para copia
    DockerVolume vol;
    vol.destination = "/.microCI_ssh";
    vol.source = sshCopyFrom;
    vol.mode = "ro";
    volumes_.insert(vol);

    data_["SSH_COPY_TO"] = sshCopyTo;
    data_["SSH_COPY_FROM"] = "/.microCI_ssh";
  }
  return {data_, volumes_};
}

// ----------------------------------------------------------------------
// Copia credencias SSH e ajusta as permissões
// ----------------------------------------------------------------------
void MicroCI::copySsh(const YAML::Node& step, const json& data) {
  mScript << inja::render(R"( \
           && cp -Rv {{ SSH_COPY_FROM }} {{ SSH_COPY_TO }} 2>&1 \
           && chmod 700 {{ SSH_COPY_TO }}/ 2>&1 \
           && chmod 644 {{ SSH_COPY_TO }}/id_rsa.pub 2>&1 \
           && chmod 600 {{ SSH_COPY_TO }}/id_rsa 2>&1)",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseBashStep(const YAML::Node& step) {
  auto cmdsStr = string{};
  auto cmds = vector<string>{};
  auto line = string{};

  auto data = defaultDataTemplate();

  if (step["bash"]) {
    cmdsStr = step["bash"].as<string>();
  } else if (step["sh"]) {
    cmdsStr = step["sh"].as<string>();
  }

  auto ss = stringstream{cmdsStr};
  while (getline(ss, line, '\n')) {
    if (!line.empty() && line.at(0) != '#') {
      cmds.push_back(line);
    }
  }

  auto volumes = parseVolumes(step);
  tie(data, volumes) = parseSsh(step, data, volumes);

  data["STEP_NAME"] = stepName(step);
  data["STEP_DESCRIPTION"] = stepDescription(step, "Executa comandos no bash");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"] = stepDockerImage(step);

  beginFunction(data);
  prepareRunDocker(data, volumes);

  if (step["sh"]) {
    mScript << inja::render("        /bin/sh -c \"cd {{ WORKSPACE }}", data);
  } else if (step["bash"]) {
    mScript << inja::render("        /bin/bash -c \"cd {{ WORKSPACE }}", data);
  } else {
    spdlog::error("Tratar erro aqui");
  }

  if (step["ssh"]) {
    copySsh(step, data);
  }

  for (auto cmd : cmds) {
    mScript << fmt::format(" \\\n           && {} 2>&1", cmd);
  }
  mScript << "\"\n";
  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
set<DockerVolume> MicroCI::defaultVolumes() const {
  set<DockerVolume> volumes{{"/microci_workspace", "${PWD}", "rw"}};

  return volumes;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
json MicroCI::defaultDataTemplate() const {
  json data;
  data["VERSION"] =
      fmt::format("{}.{}.{}    ", MAJOR, MINOR, PATCH).substr(0, 10);
  data["WORKSPACE"] = "/microci_workspace";

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
  # Modo de conformidade com POSIX
  set -o posix

  exec 5> .microCI.dbg
  BASH_XTRACEFD="5"
  PS4='$LINENO: '

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
