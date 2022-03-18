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
#include <sh/MicroCI.hpp>
#include <sh/NotifyDiscord.hpp>

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
string version() { return fmt::format("v{}.{}.{}", MAJOR, MINOR, PATCH); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::MicroCI() {
  mPluginParserMap.emplace("git_deploy", &MicroCI::parseGitDeployPluginStep);
  mPluginParserMap.emplace("git_publish", &MicroCI::parseGitPublishPluginStep);
  mPluginParserMap.emplace("mkdocs_material",
                           &MicroCI::parseMkdocsMaterialPluginStep);
  mPluginParserMap.emplace("cppcheck", &MicroCI::parseCppCheckPluginStep);
  mPluginParserMap.emplace("clang-tidy", &MicroCI::parseClangTidyPluginStep);
  mPluginParserMap.emplace("clang-format",
                           &MicroCI::parseClangFormatPluginStep);
  mPluginParserMap.emplace("plantuml", &MicroCI::parsePlantumlPluginStep);
  mPluginParserMap.emplace("beamer", &MicroCI::parseBeamerPluginStep);
  mPluginParserMap.emplace("fetch", &MicroCI::parseFetchPluginStep);

  mDockerImageGlobal = "debian:stable-slim";
  mDockerWorkspaceGlobal = "/microci_workspace";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::~MicroCI() {}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetEnvironmentVariable(EnvironmentVariable& env) {
  mEnvs.insert(env);
}

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
  } catch (const YAML::BadFile& e) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  } catch (const YAML::ParserException& e) {
    spdlog::error("Falha ao interpretar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  }

  // Variáveis de ambiente globais
  if (CI["envs"] and CI["envs"].IsMap()) {
    for (auto it : CI["envs"]) {
      EnvironmentVariable env;
      env.name = it.first.as<string>();
      env.value = it.second.as<string>();
      mEnvs.insert(env);
    }
  }

  initBash();

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
json MicroCI::parseRunAs(const YAML::Node& step, const json& data) const {
  auto data_ = data;
  data_["RUN_AS"] = "root";
  if (step["run_as"]) {
    data_["RUN_AS"] = step["run_as"].as<string>();
  }
  return data_;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
json MicroCI::parseNetwork(const YAML::Node& step, const json& data) const {
  auto data_ = data;
  if (step["network"]) {
    data_["DOCKER_NETWORK"] = step["network"].as<string>();
  }
  return data_;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::beginFunction(const json& data,
                            const set<EnvironmentVariable>& envs) {
  mScript << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  SECONDS=0
  MICROCI_STEP_NAME="{{ STEP_NAME }}"
  MICROCI_STEP_DESCRIPTION="{{ STEP_DESCRIPTION }}"
  MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "SEM GIT ORIGIN" )
  MICROCI_GIT_COMMIT=$( git rev-parse --short HEAD || echo "SEM GIT COMMIT")
  MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )
  MICROCI_STEP_STATUS=":ok:"
  MICROCI_STEP_DURATION=$SECONDS
  title="${MICROCI_STEP_NAME}.............................................................."
  title=${title:0:60}
  echo -ne "{{CYAN}}${title}{{CLEAR}}: "
)",
                          data);
  for (auto env : envs) {
    mScript << fmt::format("  {}=\"{}\"\n", env.name, env.value);
  }

  mScript << inja::render(R"(
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
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
    echo "Duration: ${MICROCI_STEP_DURATION}"
  } >> .microCI.log

  # Notificação no terminal
  if [ "${status}" = "0" ]; then
    echo -e "{{GREEN}}OK{{CLEAR}}"
  else
    echo -e "{{RED}}FALHOU{{CLEAR}}"
  fi
)",
                          data);

  auto envs = defaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    mScript << inja::render(R"(
  # Notificação via Discord
  # Usar spycolor.com para obter a cor em decimal
  if [ "${status}" = "0" ]; then
    MICROCI_STEP_STATUS=":ok:"
    MICROCI_STEP_STATUS_COLOR=4382765
  else
    MICROCI_STEP_STATUS=":face_with_symbols_over_mouth:"
    MICROCI_STEP_STATUS_COLOR=16711680
  fi
  notify_discord
)",
                            data);
  }

  mScript << "}" << endl;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::prepareRunDocker(const json& data,
                               const set<EnvironmentVariable>& envs,
                               const set<DockerVolume>& volumes) {
  const string runAs = data["RUN_AS"];

  mScript << inja::render(R"(
      echo ""
      echo ""
      echo ""
      echo "Passo: {{ STEP_NAME }}"
      # shellcheck disable=SC2140,SC2046
      docker run \)",
                          data);

  if (runAs != "root") {
    mScript << inja::render(R"(
        --user $(id -u):$(id -g) \)",
                            data);
  }

  mScript << inja::render(R"(
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --network {{ DOCKER_NETWORK }} \
        --workdir {{ WORKSPACE }})",
                          data);

  for (auto& env : envs) {
    mScript << fmt::format(R"( \
        --env {}="{}")",
                           env.name, env.value);
  }

  for (const auto& vol : volumes) {
    mScript << fmt::format(R"( \
        --volume "{}":"{}":{})",
                           vol.source, vol.destination, vol.mode);
  }

  mScript << inja::render(R"( \
        "{{ DOCKER_IMAGE }}")",
                          data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseFetchPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  data["DOCKER_NETWORK"] = "bridge";
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Baixa arquivos externos ao projeto");
  data["DOCKER_IMAGE"] = stepDockerImage(step, "bitnami/git:latest");

  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  if (step["plugin"]["items"] && step["plugin"]["items"].IsSequence()) {
    auto defaultTarget = step["plugin"]["target"].as<string>("include/");

    beginFunction(data, envs);
    prepareRunDocker(data, envs, volumes);
    mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                            data);
    copySshIfAvailable(step, data);

    for (const auto& item : step["plugin"]["items"]) {
      if (item["git_archive"]) {
        auto files = string{};
        for (const auto& f : item["files"]) {
          files += fmt::format(
              "'{}' ", f.as<string>());  // aspas simples para não expandir
        }
        if (files.empty()) {
          throw std::runtime_error(
              "É obrigatório especificar uma lista de arquivos de entrada");
        }
        data["FILES"] = files;
        data["GIT_REMOTE"] = item["git_archive"].as<string>();
        data["TARGET"] = item["target"].as<string>(defaultTarget);

        mScript << inja::render(
            R"( \
           && mkdir -p {{ TARGET }} \
           && git archive --format=tar --remote={{ GIT_REMOTE }} HEAD {{ FILES }} \
             | tar -C {{ TARGET }} -vxf - 2>&1)",
            data);
      } else if (item["url"]) {
        data["TARGET"] = item["target"].as<string>(defaultTarget);
        data["URL"] = item["url"].as<string>();
        mScript << inja::render(
            R"( \
           && mkdir -p {{ TARGET }} \
           && pushd {{ TARGET }} \
           && curl -fSL -R -J -O {{ URL }} 2>&1 \
           && popd)",
            data);
      }
    }
    mScript << "\"\n";
    endFunction(data);
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseBeamerPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Apresentação PDF criada a partir do markdown");
  data["DOCKER_IMAGE"] = stepDockerImage(step, "pandoc/latex:latest");
  data["WORKSPACE"] = stepDockerWorkspace(step, "/data");

  auto inputMD = string{};
  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& opt : step["plugin"]["source"]) {
      inputMD += opt.as<string>() + " ";
    }
  }
  if (!inputMD.empty()) {
    data["INPUT_MD"] = inputMD;
  } else {
    spdlog::error("É obrigatório especificar uma lista de arquivos de entrada");
    throw std::runtime_error(
        "É obrigatório especificar uma lista de arquivos de entrada");
  }

  data["LANG"] = step["plugin"]["lang"].as<string>("pt-BR");
  data["DATE"] = step["plugin"]["date"].as<string>("01 de Abril de 2023");
  data["INSTITUTE"] =
      step["plugin"]["institute"].as<string>("Nome da instituição");
  data["TITLE"] = step["plugin"]["title"].as<string>("Título da apresentação");
  data["SUBTITLE"] =
      step["plugin"]["subtitle"].as<string>("Subtítulo da apresentação");
  data["SUBJECT"] = step["plugin"]["subject"].as<string>(
      "Informação da propriedade Assunto do PDF");
  data["SLIDE_LEVEL"] = step["plugin"]["slide-level"].as<string>("2");
  data["ASPECTRATIO"] = step["plugin"]["aspectratio"].as<string>("169");
  data["OUTPUT_PDF"] = step["plugin"]["output"].as<string>("output.pdf");

  data["HEADER_INCLUDES"] = "";
  auto filename = string{"header-includes.yaml"};
  ofstream yaml(filename);
  YAML::Node strippedHeaderIncludes = Clone(step["plugin"]);

  if (step["plugin"]["theme"].as<string>("default") == "STR") {
    strippedHeaderIncludes["header-includes"].push_back(
        R"(\useoutertheme{shadow})");
    strippedHeaderIncludes["header-includes"].push_back(
        R"(\usecolortheme{str})");
    strippedHeaderIncludes["header-includes"].push_back(
        R"(\logo{\includegraphics[height=7mm]{img/str-logo.png}})");

// Encontrar uma solução mais elegante para disponibilizar arquivos de terceiros
#include <3rd/beamercolorthemestr.sty.hpp>
#include <3rd/str-logo.png.hpp>
    ofstream strLogoPng("img/str-logo.png");
    strLogoPng.write((char*)___3rd_str_logo_png, ___3rd_str_logo_png_len);
    ofstream beamercolorthemestr("beamercolorthemestr.sty");
    beamercolorthemestr.write((char*)___3rd_beamercolorthemestr_sty,
                              ___3rd_beamercolorthemestr_sty_len);
  }

  for (const auto& key :
       {/* keys a seguir não são usadas pelo pandoc: */ "name", "output",
        "source", "theme",
        /* keys a seguir já foram passadas via linha de comando: */ "lang",
        "date", "institute", "title", "subtitle", "subject", "slide-level",
        "aspectratio"}) {
    strippedHeaderIncludes.remove(key);
  }

  yaml << "---\n";
  yaml << "# Atenção, este arquivo é gerado automaticamente!\n";
  yaml << "# Se necessário edite o arquivo .microCI.yml\n";
  yaml << strippedHeaderIncludes;
  yaml << "\n..." << endl;
  data["HEADER_INCLUDES"] = filename;

  if (step["plugin"]["natbib"] and step["plugin"]["natbib"].as<bool>()) {
    data["CITEPROC"] = "--citeproc";
  } else {
    data["CITEPROC"] = "";
  }

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

  mScript << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --workdir {{ WORKSPACE }} \
        --volume "${PWD}":{{ WORKSPACE }} \
        --user $(id -u):$(id -g) \
        {{ DOCKER_IMAGE }} \
        --variable lang='{{ LANG }}' \
        --variable date='{{ DATE }}' \
        --variable institute='{{ INSTITUTE }}' \
        --variable title='{{ TITLE }}' \
        --variable subtitle='{{ SUBTITLE }}' \
        --variable subject='{{ SUBJECT }}' \
        --variable aspectratio={{ ASPECTRATIO }} \
        --slide-level={{ SLIDE_LEVEL }} \
        --to=beamer \
        {{ CITEPROC }} \
        {{ HEADER_INCLUDES }} \
        {{ INPUT_MD }} \
        -o {{ OUTPUT_PDF }} \
        2>&1; \
      rm -f {{ HEADER_INCLUDES }} img/str-logo.png beamercolorthemestr.sty 2>&1
)",
                          data);

  endFunction(data);
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
  data["DOCKER_IMAGE"] = "intmain/microci_mkdocs_material:latest";

  // https://unix.stackexchange.com/questions/155551/how-to-debug-a-bash-script
  // exec 5> >(logger -t $0)
  // BASH_XTRACEFD="5"
  // PS4='$LINENO: '
  // set -x
  //
  // # Place your code here

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

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
        mkdocs {{ ACTION }} 2>&1
)",
                          data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parsePlantumlPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  auto runAs = string{};
  auto type = string{"png"};
  auto output = string{};
  list<string> sourceList;
  list<string> opts = {"-r"};

  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto& opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  if (step["plugin"]["type"]) {
    type = step["plugin"]["type"].as<string>();
  }

  if (step["plugin"]["output"]) {
    output = step["plugin"]["output"].as<string>();
  }

  // para executar com GUI
  // -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/

  opts.push_back("-t" + type);
  if (!output.empty()) {
    opts.push_back("-o " + output);
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] =
      stepDockerImage(step, "intmain/microci_plantuml:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Constroi diagramas plantuml");
  data["OUTPUT"] = output;

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mScript << inja::render(R"( \
          /bin/bash -c "cd {{ WORKSPACE }} \
          && java -jar /opt/plantuml/plantuml.jar \
)",
                          data);

  for (const auto& opt : opts) {
    mScript << "            " << opt << " \\\n";
  }

  for (const auto& src : sourceList) {
    mScript << "            " << src << " \\\n";
  }

  mScript << inja::render(
      R"(            2>&1"
)",
      data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseClangTidyPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  auto runAs = string{};
  list<string> includeList;
  list<string> sourceList;
  list<string> opts{"--"};

  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto& opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["include"] && step["plugin"]["include"].IsSequence()) {
    for (const auto& inc : step["plugin"]["include"]) {
      includeList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] =
      stepDockerImage(step, "intmain/microci_cppcheck:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Verifica código C++");

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }} \
        && mkdir -p auditing/clang-tidy/ \
        && date > auditing/clang-tidy/clang-tidy.log \
        && clang-tidy \
)",
                          data);
  for (const auto& src : sourceList) {
    mScript << "        " << src << " \\\n";
  }

  mScript << "        -checks='*' \\\n";

  for (const auto& opt : opts) {
    mScript << "        " << opt << " \\\n";
  }

  for (const auto& inc : includeList) {
    mScript << "        -I" << inc << " \\\n";
  }

  mScript << inja::render(
      R"(        2>&1 | tee auditing/clang-tidy/clang-tidy.log 2>&1 \
       && clang-tidy-html auditing/clang-tidy/clang-tidy.log 2>&1 \
       && mv -v clang-tidy-checks.py auditing/clang-tidy/ 2>&1 \
       && mv -v clang.html auditing/clang-tidy/index.html 2>&1 \
       && chown $(id -u):$(id -g) -Rv auditing 2>&1"
)",
      data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseClangFormatPluginStep(const YAML::Node& step) {
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  list<string> sourceList;

  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] =
      stepDockerImage(step, "intmain/microci_cppcheck:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Formata código C++");
  data["RUN_AS"] = "user";

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                          data);
  for (const auto& src : sourceList) {
    mScript << fmt::format(R"( \
        && cat <(compgen -G '{}') \
          | )",
                           src);
    mScript << R"(xargs -I {} clang-format -i {} 2>&1 )";
  }

  mScript << R"("
)";
  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseCppCheckPluginStep(const YAML::Node& step) {
  auto platform = string{"unix64"};
  auto standard = string{"c++11"};
  list<string> includeList;
  list<string> sourceList;
  list<string> opts{"--enable=all", "--inconclusive", "--xml",
                    "--xml-version=2"};

  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto& opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["include"] && step["plugin"]["include"].IsSequence()) {
    for (const auto& inc : step["plugin"]["include"]) {
      includeList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& src : step["plugin"]["source"]) {
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
  data["DOCKER_IMAGE"] =
      stepDockerImage(step, "intmain/microci_cppcheck:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Verifica código C++");
  data["PLATFORM"] = platform;
  data["STD"] = standard;
  data["REPORT_TITLE"] = "MicroCI::CppCheck";
  // data["RUN_AS"] = "user";

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }} \
        && mkdir -p auditing/cppcheck \
        && cppcheck \
          --platform={{ PLATFORM }} \
          --std={{ STD }} \
)",
                          data);

  for (const auto& opt : opts) {
    mScript << "          " << opt << " \\\n";
  }

  for (const auto& inc : includeList) {
    mScript << "          --include=" << inc << " \\\n";
  }

  for (const auto& src : sourceList) {
    mScript << "          " << src << " \\\n";
  }

  // xml é escrito na saída de erro
  mScript << inja::render(R"(          2> auditing/cppcheck.xml \
        && cppcheck-htmlreport \
          --title='{{ REPORT_TITLE }}' \
          --report-dir='auditing/cppcheck/' \
          --source-dir='.' \
          --file='auditing/cppcheck.xml' 2>&1 \
        && chown $(id -u):$(id -g) -Rv auditing 2>&1"
)",
                          data);

  endFunction(data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parseGitPublishPluginStep(const YAML::Node& step) {
  auto pluginCopyTo = string{"/microci_deploy"};
  auto pluginCopyFrom = string{"site"};
  auto cleanBefore = true;
  auto data = defaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);

  data["DOCKER_NETWORK"] = "bridge";
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  const auto name = step["plugin"]["name"].as<string>();
  const auto gitURL = step["plugin"]["git_url"].as<string>();

  if (step["plugin"]["copy_from"]) {
    pluginCopyFrom = step["plugin"]["copy_from"].as<string>();
  }

  if (step["plugin"]["copy_to"]) {
    pluginCopyTo = step["plugin"]["copy_to"].as<string>();
  }

  if (step["plugin"]["clean_before"]) {
    cleanBefore = step["plugin"]["clean_before"].as<bool>();
  }

  data["GIT_URL"] = gitURL;
  data["PLUGIN_COPY_TO"] = pluginCopyTo;
  data["PLUGIN_COPY_FROM"] = pluginCopyFrom;
  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "bitnami/git:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Publica arquivos em um repositório git");

  // data["RUN_AS"] = "user"; // verificar!
  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                          data);

  copySshIfAvailable(step, data);

  mScript << inja::render(R"( \
           && git clone '{{ GIT_URL }}' --depth 1 '{{ PLUGIN_COPY_TO }}' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} config user.name  '$(git config --get user.name)' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} config user.email '$(git config --get user.email)' 2>&1 \)",
                          data);

  if (cleanBefore) {
    mScript << inja::render(R"(
           && git -C {{ PLUGIN_COPY_TO }} rm '*' 2>&1 \)",
                            data);
  }

  mScript << inja::render(R"(
           && cp -rv {{ PLUGIN_COPY_FROM }}/* {{ PLUGIN_COPY_TO }}/ 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} add . 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} commit -am ':rocket:microCI git_publish' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} push origin master 2>&1 \
           && chown $(id -u):$(id -g) -Rv {{ PLUGIN_COPY_FROM }} 2>&1
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
  auto clean = true;

  if (step["plugin"]["clean"]) {
    clean = step["plugin"]["clean"].as<bool>();
  }

  auto data = defaultDataTemplate();
  data["RUN_AS"] = "root";
  data["GIT_URL"] = repo;
  data["GIT_DIR"] = gitDir;
  data["GIT_WORK"] = workTree;
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step);

  auto envs = parseEnvs(step);

  beginFunction(data, envs);
  mScript << inja::render(R"(
      # Caso ainda não exista realiza o clone inicial
      if [ ! -d "{{GIT_DIR}}" ]; then
        git clone "{{GIT_URL}}" \
          --separate-git-dir="{{GIT_DIR}}" \
          "{{GIT_WORK}}" 2>&1
      fi

)",
                          data);

  if (clean) {
    mScript << inja::render(R"(
      # Limpa a pasta -- CUIDADO AO MESCLAR REPOS
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        clean -xfd 2>&1
)",
                            data);
  }

  mScript << inja::render(R"(
      # Extrai a versão atual
      git --git-dir="{{GIT_DIR}}" \
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
set<EnvironmentVariable> MicroCI::parseEnvs(const YAML::Node& step) const {
  auto ret = defaultEnvs();
  if (step["envs"] and step["envs"].IsMap()) {
    for (auto it : step["envs"]) {
      EnvironmentVariable env;
      env.name = it.first.as<string>();
      env.value = it.second.as<string>();
      ret.insert(env);
      spdlog::info("Environment variable: {} -> {}", env.name, env.value);
    }
  }
  return ret;
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
string MicroCI::stepDockerWorkspace(const YAML::Node& step,
                                    const string& workspace) const {
  string dockerWorkspace = mDockerWorkspaceGlobal;

  if (!workspace.empty()) {
    dockerWorkspace = workspace;
  }

  if (step["workspace"]) {
    dockerWorkspace = step["workspace"].as<string>();
  }

  return dockerWorkspace;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
tuple<json, set<DockerVolume>, set<EnvironmentVariable>> MicroCI::parseSsh(
    const YAML::Node& step, const json& data, const set<DockerVolume>& volumes,
    const set<EnvironmentVariable>& envs) const {
  auto sshMountForCopy = string{"${HOME}/.ssh"};
  auto volumes_ = volumes;
  auto data_ = data;
  auto envs_ = envs;

  data_["SSH_COPY_TO"] = string{};
  data_["SSH_COPY_FROM"] = "/.microCI_ssh";

  if (step["ssh"]) {
    // o copy_from será montado em /.microCI_ssh
    if (step["ssh"]["copy_from"]) {
      sshMountForCopy = step["ssh"]["copy_from"].as<string>();
    }

    if (step["ssh"]["copy_to"]) {
      data_["SSH_COPY_TO"] = step["ssh"]["copy_to"].as<string>();
    } else {
      auto gitSshCommandEnv = EnvironmentVariable{
          "GIT_SSH_COMMAND",
          "ssh -i /.microCI_ssh/id_rsa"
          " -F /dev/null"
          " -o UserKnownHostsFile=/.microCI_ssh/known_hosts"};
      envs_.insert(gitSshCommandEnv);
    }

    // Montagem temporária para copia
    DockerVolume vol;
    vol.destination = "/.microCI_ssh";
    vol.source = sshMountForCopy;
    vol.mode = "ro";
    volumes_.insert(vol);
  }
  return {data_, volumes_, envs_};
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::copySshIfAvailable(const YAML::Node& step, const json& data) {
  if (!step["ssh"] or !step["ssh"]["copy_to"]) {
    return;
  }

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
  auto runAs = string{};

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
  auto envs = parseEnvs(step);
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  data["STEP_NAME"] = stepName(step);
  data["STEP_DESCRIPTION"] = stepDescription(step, "Executa comandos no bash");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"] = stepDockerImage(step);

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  if (step["sh"]) {
    mScript << inja::render(R"( \
        /bin/sh -c "cd {{ WORKSPACE }})",
                            data);
  } else if (step["bash"]) {
    mScript << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                            data);
  } else {
    spdlog::error("Tratar erro aqui");
  }

  copySshIfAvailable(step, data);

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
set<EnvironmentVariable> MicroCI::defaultEnvs() const { return mEnvs; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
json MicroCI::defaultDataTemplate() const {
  json data;
  data["VERSION"] =
      fmt::format("{}.{}.{}       ", MAJOR, MINOR, PATCH).substr(0, 10);
  data["WORKSPACE"] = mDockerWorkspaceGlobal;

  // Network docker: bridge (default), host, none
  data["DOCKER_NETWORK"] = "none";
  data["DOCKER_IMAGE"] = mDockerImageGlobal;
  data["RUN_AS"] = "root";

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
  auto scriptMicroCI = string{reinterpret_cast<const char*>(___sh_MicroCI_sh),
                              ___sh_MicroCI_sh_len};
  mScript << inja::render(scriptMicroCI, data) << endl;

  auto envs = defaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    auto scriptNotifyDiscord =
        string{reinterpret_cast<const char*>(___sh_NotifyDiscord_sh),
               ___sh_NotifyDiscord_sh_len};

    mScript << inja::render(scriptNotifyDiscord, data) << endl;
  } else {
    mScript << "# Notificação via Discord não será possível" << endl;
  }
}

}  // namespace microci
