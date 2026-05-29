// MIT License
//
// Copyright (C) 2022-2026 Geraldo Luis da Silva Ribeiro
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

#include "MicroCI.hpp"

#include <pwd.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "inja/inja.hpp"
#include "sh/MicroCI.hpp"
#include "sh/NotifyDiscord.hpp"

// Resolve forward definition
#include "PluginStepParser.hpp"
#include "SkipPluginStepParser.hpp"

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
MicroCI::MicroCI() {
  mDefaultDockerImage = "debian:stable-slim";
  mDefaultWorkspace   = "/microci_workspace";
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
auto MicroCI::IsValid() const -> bool { return mIsValid; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::invalidConfigurationDetected() { mIsValid = false; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::List(const std::string &filename) const -> std::string {
  std::string ret;

  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);
    if (CI["steps"].IsSequence()) {
      std::size_t number = 1;
      for (auto step : CI["steps"]) {
        auto name = step["name"].as<std::string>();
        auto hhh  = std::hash<std::string>{}(name) & 0xffff;
        ret += fmt::format("{:>2} {:04x} {}\n", number++, hhh, name);
      }
    }
  } catch (const YAML::BadFile &e) {
    spdlog::error("Failure loading the file .microCI.yml");
    spdlog::error(e.what());
  } catch (const YAML::ParserException &e) {
    spdlog::error("Failure parsing the file .microCI.yml");
    spdlog::error(e.what());
  }

  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ActivityDiagram(const std::string &filename) const -> std::string {
  std::string ret;

  std::string beginDiagram{R"(

' Generated Diagram! Do not edit
@startuml
start
)"};

  std::string endDiagram{R"(
stop
@enduml
)"};

  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);

    if (CI["steps"].IsSequence()) {
      ret += beginDiagram;
      ret += "partition \"Main pipeline\" {\n";
      for (auto step : CI["steps"]) {
        if (!step["only"]) {
          std::string stereotype{"<<procedure>>"};
          std::string plugin{"No plugin name available"};
          if (step["plugin"] and step["plugin"]["name"]) {
            plugin = step["plugin"]["name"].as<std::string>();
            if (plugin == "fetch") {
              stereotype = "<<input>>";
            }
          }
          std::string description{"No description available"};
          if (step["description"]) {
            description = step["description"].as<std::string>();
          }

          ret += fmt::format(":{}; {}\n", step["name"].as<std::string>(), stereotype);
          ret += fmt::format("floating note left: {}\n", plugin);
          ret += fmt::format("floating note right: {}\n\n", description);
        }
      }
      ret += "}\n";
      ret += endDiagram;

      for (auto step : CI["steps"]) {
        if (step["only"]) {
          std::string stereotype{"<<procedure>>"};
          std::string plugin{"No plugin name available"};
          if (step["plugin"] and step["plugin"]["name"]) {
            plugin = step["plugin"]["name"].as<std::string>();
            if (plugin == "fetch") {
              stereotype = "<<input>>";
            }
          }
          std::string description{"No description available"};
          if (step["description"]) {
            description = step["description"].as<std::string>();
          }

          ret += beginDiagram;
          ret += "partition \"Alternative flow\" {\n";
          ret += fmt::format(":{}; {}\n", step["name"].as<std::string>(), stereotype);
          ret += fmt::format("floating note left: {}\n", plugin);
          ret += fmt::format("floating note right: {}\n\n", description);
          ret += "}\n";
          ret += endDiagram;
        }
      }
    }
  } catch (const YAML::BadFile &e) {
    spdlog::error("Failure loading the file .microCI.yml");
    spdlog::error(e.what());
  } catch (const YAML::ParserException &e) {
    spdlog::error("Failure parsing the file .microCI.yml");
    spdlog::error(e.what());
  }

  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::RegisterPlugin(const std::string &name, std::shared_ptr<PluginStepParser> pluginStepParser) {
  mPluginParserMap[name] = pluginStepParser;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::Script() -> std::stringstream & {
  if (IsValid()) {
    return mScript;
  }
  throw std::runtime_error("Invalid configuration detected");
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultDockerImage() const -> std::string { return mDefaultDockerImage; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultWorkspace() const -> std::string { return mDefaultWorkspace; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetEnvironmentVariable(const EnvironmentVariable &env) { mEnvs.insert(env); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetAltHome(const std::string &altHome) { mAltHome = altHome; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetOnlyStep(const std::string &onlyStep) { mOnlyStep = onlyStep; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetOnlyStepNumber(const std::size_t onlyStepNumber) { mOnlyStepNumber = onlyStepNumber; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetOnlyStepHash(const std::string &filename, const std::string &hh) {
  YAML::Node CI;
  try {
    CI = YAML::LoadFile(filename);
    if (CI["steps"].IsSequence()) {
      std::size_t number = 1;
      mOnlyStepNumber    = std::nullopt;
      for (auto step : CI["steps"]) {
        auto name = step["name"].as<std::string>();
        auto hhi  = fmt::format("{:04x}", std::hash<std::string>{}(name) & 0xffff);
        if (hh == hhi) {
          mOnlyStepNumber = number;
          return;
        }
        number++;
      }
      throw std::invalid_argument(fmt::format("Invalid step hash: {}", hh));
    }
  } catch (const YAML::BadFile &e) {
    spdlog::error("Failure loading the file .microCI.yml");
    spdlog::error(e.what());
  } catch (const YAML::ParserException &e) {
    spdlog::error("Failure parsing the file .microCI.yml");
    spdlog::error(e.what());
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::SetAppendLog(const bool appendLog) { mAppendLog = appendLog; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ToString() const -> std::string {
  if (IsValid()) {
    return mScript.str();
  }
  throw std::runtime_error("Invalid configuration detected");
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::AddDockerImage(const std::string &image) { mDockerImages.insert(image); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::LoadEnvironmentFromYamlFile(const std::string &filename) {
  if (std::filesystem::exists(filename)) {
    auto dotEnv = YAML::LoadFile(filename);
    if (dotEnv.size() == 0) {
      spdlog::error("The file {} was found but it has no valid configuration", filename);
      invalidConfigurationDetected();
      // return false;
    }
    for (auto it : dotEnv) {
      EnvironmentVariable env;
      env.name  = it.first.as<std::string>();
      env.value = it.second.as<std::string>();
      mEnvs.insert(env);
    }
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::LoadEnvironmentFromEnvFile(const std::string &filename) {
  if (std::filesystem::exists(filename)) {
    std::ifstream envFile(filename);
    std::string line;
    while (getline(envFile, line)) {
      // Skip comments and short lines
      if (line.size() < 3 or line[0] == '#') {
        continue;
      }
      auto i = 0;
      while (line.at(i) == ' ' or line.at(i) == '\t') {
        ++i;
        if (line[i] == '#') {
          line = "";
          break;
        }
      }
      auto eqPos = line.find_first_of("=");
      if (eqPos != std::string::npos) {
        EnvironmentVariable env;
        env.name  = line.substr(0, eqPos);
        env.value = line.substr(eqPos + 1);
        mEnvs.insert(env);
      }
    }
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ReadConfig(const std::string &filename) -> bool {
  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);

    // Global environment configuration
    if (mAltHome.empty()) {
      struct passwd *pw = getpwuid(getuid());
      if (pw) {
        auto globalEnvironmentFilename = fmt::format("{}/.microCI.env", pw->pw_dir);
        LoadEnvironmentFromEnvFile(globalEnvironmentFilename);
      }
    } else {
      auto globalEnvironmentFilename = fmt::format("{}/.microCI.env", mAltHome);
      LoadEnvironmentFromEnvFile(globalEnvironmentFilename);
    }

    // Start with global environment variables
    if (CI["envs"] and CI["envs"].IsMap()) {
      for (auto it : CI["envs"]) {
        EnvironmentVariable env;
        env.name  = it.first.as<std::string>();
        env.value = it.second.as<std::string>();
        mEnvs.insert(env);
      }
    }

    mYamlFilename = filename;

    LoadEnvironmentFromYamlFile(".env.yml");
    LoadEnvironmentFromEnvFile(".env");

  } catch (const YAML::BadFile &e) {
    spdlog::error("Failure loading the file .microCI.yml");
    spdlog::error(e.what());
    invalidConfigurationDetected();
    return false;
  } catch (const YAML::ParserException &e) {
    spdlog::error("Failure parsing the file .microCI.yml");
    spdlog::error(e.what());
    invalidConfigurationDetected();
    return false;
  }

  initBash(CI);

  // Imagem docker global (opcional)
  if (CI["docker"].IsScalar()) {
    mDefaultDockerImage = CI["docker"].as<std::string>();
  }

  if (mOnlyStepNumber) {
    if (CI["steps"].size() < mOnlyStepNumber.value() or mOnlyStepNumber.value() < 1) {
      throw std::invalid_argument(fmt::format("Invalid step number: {}", mOnlyStepNumber.value()));
    }
    auto step = CI["steps"][mOnlyStepNumber.value() - 1];
    if (!step or !step["plugin"] or !step["plugin"]["name"]) {
      throw std::invalid_argument(fmt::format("Plugin not defined at the step '{}'", step["name"].as<std::string>()));
    }
    parsePluginStep(step);
    mScript << "# Execute step #" << mOnlyStepNumber.value() << std::endl;
    mScript << "step_" << sanitizeName(step["name"].as<std::string>()) << std::endl;
    mScript << "exit 0;" << std::endl;
  } else if (!mOnlyStep.empty()) {
    // FIXME: Verificar se existe
    for (auto step : CI["steps"]) {
      if (!step["plugin"] or !step["plugin"]["name"]) {
        throw std::invalid_argument(fmt::format("Plugin not defined at the step '{}'", step["name"].as<std::string>()));
      }
      if (step["only"] and step["only"].as<std::string>() == mOnlyStep) {
        parsePluginStep(step);
        mScript << "# Execute only this step" << std::endl;
        mScript << "step_" << sanitizeName(step["name"].as<std::string>()) << std::endl;
        mScript << "exit 0;" << std::endl;
      }
    }
  } else {
    if (CI["steps"].IsSequence()) {
      for (auto step : CI["steps"]) {
        if (!step["plugin"] or !step["plugin"]["name"]) {
          throw std::invalid_argument(
              fmt::format("Plugin not defined at the step '{}'", step["name"].as<std::string>()));
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
        mScript << "echo 'Updating docker images...'\n";
        for (const auto &dockerImage : mDockerImages) {
          mScript << fmt::format("  echo 'Updating {} docker image...' >> .microCI.log\n", dockerImage);
          mScript << fmt::format("  docker pull {} --quiet 2>&1 >> .microCI.log\n", dockerImage);
        }
      }

      mScript << R"(

# Execute all steps in the pipeline
function main() {
  date >> .microCI.log

)";
      for (auto step : CI["steps"]) {
        // if (step["only"]) {
        //   continue;
        // }
        auto stepName = step["name"].as<std::string>();
        mScript << "  step_" << sanitizeName(stepName) << std::endl;
      }
      mScript << R"(
  date >> .microCI.log
}

# Entry point
main

# Usage
# -----
#
# To execute this workflow inside a terminal use the following command:
# microCI | bash
#
# To save the workflow as a bash scritp just redirect the output to a file:
# microCI > build.sh

)";
    }
  }
  return true;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::parsePluginStep(const YAML::Node &step) {
  auto pluginName = step["plugin"]["name"].as<std::string>();

  if (mPluginParserMap.count(pluginName)) {
    mPluginParserMap.at(pluginName)->Parse(step);
    if (!mPluginParserMap.at(pluginName)->IsValid()) {
      invalidConfigurationDetected();
    }
    return;
  } else {
    auto stepName = step["name"].as<std::string>();
    spdlog::error("The plugin '{}' was not found at the step '{}'", pluginName, stepName);
    for (const auto &p : mPluginParserMap) {
      spdlog::warn("Plugin '{}'", p.first);
    }
    return;
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultVolumes() const -> std::set<DockerVolume> {
  std::set<DockerVolume> volumes{{"/microci_workspace", "${MICROCI_PWD}", "rw"}};
  return volumes;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultEnvs() const -> std::set<EnvironmentVariable> { return mEnvs; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultDataTemplate() const -> json {
  json data;
  data["VERSION"]           = fmt::format("v{}       ", microCI_version).substr(0, 10);
  data["WORKSPACE"]         = mDefaultWorkspace;
  data["DOCKER_NETWORK"]    = "none";
  data["DOCKER_IMAGE"]      = mDefaultDockerImage;
  data["RUN_AS"]            = "root";
  data["MICROCI_STEP_SKIP"] = "no";
  data["MICROCI_YAML"]      = mYamlFilename;

  data["BLUE"]    = "\033[0;34m";
  data["YELLOW"]  = "\033[0;33m";
  data["MAGENTA"] = "\033[0;35m";
  data["RED"]     = "\033[0;31m";
  data["GREEN"]   = "\033[0;32m";
  data["CYAN"]    = "\033[0;36m";
  data["CLEAR"]   = "\033[0m";

  data["APPEND_LOG_TEE_FLAG"] = mAppendLog ? " -a" : "";

#ifdef __APPLE__
  data["RANDOM_8"] = "$(uuidgen | head -c 8)";
#endif

#ifdef __linux__
  data["RANDOM_8"] = "$(head -c 8 /proc/sys/kernel/random/uuid)";
#endif

  return data;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::initBash(const YAML::Node &CI) {
  auto data = DefaultDataTemplate();

  std::stringstream stepsComments;

  if (CI["steps"] and CI["steps"].IsMap()) {
    for (auto step : CI["steps"]) {
      if (step["only"]) {
        continue;
      }
      if (step["name"]) {
        stepsComments << "# Step: " << step["name"].as<std::string>() << "\n";
      }
      if (step["description"]) {
        stepsComments << "#       " << step["description"].as<std::string>() << "\n\n";
      }
    }
  }
  data["STEPS_COMMENTS"] = stepsComments.str();

  if (mOnlyStepNumber) {
    data["MICROCI_STEP_NUMBER"] = mOnlyStepNumber.value() - 1;
  } else {
    data["MICROCI_STEP_NUMBER"] = 0;
  }

  auto scriptMicroCI = std::string{reinterpret_cast<const char *>(___sh_MicroCI_sh), ___sh_MicroCI_sh_len};
  mScript << inja::render(scriptMicroCI, data) << std::endl;

  auto envs       = DefaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    auto scriptNotifyDiscord =
        std::string{reinterpret_cast<const char *>(___sh_NotifyDiscord_sh), ___sh_NotifyDiscord_sh_len};

    mScript << inja::render(scriptNotifyDiscord, data) << std::endl;
  } else {
    mScript << "# Notification by Discord is not possible" << std::endl;
  }
}

}  // namespace microci
