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

// Resolve forward definition
#include <PluginStepParser.hpp>
#include <SkipPluginStepParser.hpp>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::MicroCI() {
  mDefaultDockerImage = "debian:stable-slim";
  mDefaultWorkspace = "/microci_workspace";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::~MicroCI() {
  // verificar se precisa deletar mPluginParserMap
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
bool MicroCI::IsValid() const { return mIsValid; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::RegisterPlugin(const string &name, shared_ptr<PluginStepParser> pluginStepParser) {
  mPluginParserMap[name] = pluginStepParser;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
stringstream &MicroCI::Script() { return mScript; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::DefaultDockerImage() const { return mDefaultDockerImage; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::DefaultWorkspace() const { return mDefaultWorkspace; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetEnvironmentVariable(const EnvironmentVariable &env) { mEnvs.insert(env); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetOnlyStep(const string &onlyStep) { mOnlyStep = onlyStep; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string MicroCI::ToString() const { return mScript.str(); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::AddDockerImage(const string &image) { mDockerImages.insert(image); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
bool MicroCI::ReadConfig(const string &filename) {
  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);

    // Variáveis de ambiente globais
    if (CI["envs"] and CI["envs"].IsMap()) {
      for (auto it : CI["envs"]) {
        EnvironmentVariable env;
        env.name = it.first.as<string>();
        env.value = it.second.as<string>();
        mEnvs.insert(env);
      }
    }

    // Sobrescreve com .env
    if (filesystem::exists(".env")) {
      auto dotEnv = YAML::LoadFile(".env");
      for (auto it : dotEnv) {
        EnvironmentVariable env;
        env.name = it.first.as<string>();
        env.value = it.second.as<string>();
        mEnvs.insert(env);
      }
    }

  } catch (const YAML::BadFile &e) {
    spdlog::error("Falha ao carregar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  } catch (const YAML::ParserException &e) {
    spdlog::error("Falha ao interpretar o arquivo .microCI.yml");
    spdlog::error(e.what());
    return false;
  }

  initBash();

  // Imagem docker global (opcional)
  if (CI["docker"].IsScalar()) {
    mDefaultDockerImage = CI["docker"].as<string>();
  }

  if (!mOnlyStep.empty()) {
    // FIXME: Verificar se existe
    for (auto step : CI["steps"]) {
      if (!step["plugin"] or !step["plugin"]["name"]) {
        throw invalid_argument(fmt::format("Plugin não definido no passo '{}'", step["name"].as<string>()));
      }
      if (step["only"] and step["only"].as<string>() == mOnlyStep) {
        parsePluginStep(step);
        mScript << "# Executa somente este passo" << endl;
        mScript << "step_" << sanitizeName(step["name"].as<string>()) << endl;
        mScript << "exit 0;" << endl;
      }
    }
  } else {
    if (CI["steps"].IsSequence()) {
      for (auto step : CI["steps"]) {
        if (!step["plugin"] or !step["plugin"]["name"]) {
          throw invalid_argument(fmt::format("Plugin não definido no passo '{}'", step["name"].as<string>()));
        }
        if (step["only"]) {
          SkipPluginStepParser skipPluginStepParser{this};
          skipPluginStepParser.Parse(step);
        } else {
          parsePluginStep(step);
        }
      }

      if (!mDefaultDockerImage.empty()) {
        mDockerImages.insert(mDefaultDockerImage);
      }

      if (mDockerImages.size()) {
        mScript << "# Atualiza as imagens docker utilizadas no passos\n";
        for (const auto &dockerImage : mDockerImages) {
          mScript << fmt::format("echo 'Atualizando imagem docker {}...'\n", dockerImage);
          mScript << fmt::format("docker pull {} 2>&1 > .microCI.log\n", dockerImage);
        }
      }

      mScript << R"(

# Executa todos os passos do pipeline
function main() {
  date >> .microCI.log

)";
      for (auto step : CI["steps"]) {
        // if (step["only"]) {
        //   continue;
        // }
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
void MicroCI::parsePluginStep(const YAML::Node &step) {
  auto pluginName = step["plugin"]["name"].as<string>();

  if (mPluginParserMap.count(pluginName)) {
    mPluginParserMap.at(pluginName)->Parse(step);
    mIsValid = mIsValid && mPluginParserMap.at(pluginName)->IsValid();
    return;
  } else {
    auto stepName = step["name"].as<string>();
    spdlog::error("O plugin '{}' não foi encontrado no passo '{}'", pluginName, stepName);
    for (const auto p : mPluginParserMap) {
      spdlog::warn("Plugin '{}'", p.first);
    }
    return;
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
set<DockerVolume> MicroCI::DefaultVolumes() const {
  set<DockerVolume> volumes{{"/microci_workspace", "${MICROCI_PWD}", "rw"}};
  return volumes;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
set<EnvironmentVariable> MicroCI::DefaultEnvs() const { return mEnvs; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
json MicroCI::DefaultDataTemplate() const {
  json data;
  data["VERSION"] = fmt::format("{}.{}.{}       ", MAJOR, MINOR, PATCH).substr(0, 10);
  data["WORKSPACE"] = mDefaultWorkspace;

  // Network docker: bridge (default), host, none
  data["DOCKER_NETWORK"] = "none";
  data["DOCKER_IMAGE"] = mDefaultDockerImage;
  data["RUN_AS"] = "root";
  data["MICROCI_STEP_SKIP"] = "no";

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
  auto data = DefaultDataTemplate();
  auto scriptMicroCI = string{reinterpret_cast<const char *>(___sh_MicroCI_sh), ___sh_MicroCI_sh_len};
  mScript << inja::render(scriptMicroCI, data) << endl;

  auto envs = DefaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    auto scriptNotifyDiscord =
        string{reinterpret_cast<const char *>(___sh_NotifyDiscord_sh), ___sh_NotifyDiscord_sh_len};

    mScript << inja::render(scriptNotifyDiscord, data) << endl;
  } else {
    mScript << "# Notificação via Discord não será possível" << endl;
  }
}

}  // namespace microci
