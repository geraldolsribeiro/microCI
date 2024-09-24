// MIT License
//
// Copyright (C) 2022-2024 Geraldo Luis da Silva Ribeiro
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

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

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
auto MicroCI::IsValid() const -> bool { return mIsValid; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::invalidConfigurationDetected() { mIsValid = false; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ActivityDiagram(const string &filename) const -> string {
  string ret;

  string beginDiagram{R"(

' Generated Diagram! Do not edit
@startuml
start
)"};

  string endDiagram{R"(
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
          string stereotype{"<<procedure>>"};
          string plugin{"No plugin name available"};
          if (step["plugin"] and step["plugin"]["name"]) {
            plugin = step["plugin"]["name"].as<string>();
            if (plugin == "fetch") {
              stereotype = "<<input>>";
            }
          }
          string description{"No description available"};
          if (step["description"]) {
            description = step["description"].as<string>();
          }

          ret += fmt::format(":{}; {}\n", step["name"].as<string>(), stereotype);
          ret += fmt::format("floating note left: {}\n", plugin);
          ret += fmt::format("floating note right: {}\n\n", description);
        }
      }
      ret += "}\n";
      ret += endDiagram;

      for (auto step : CI["steps"]) {
        if (step["only"]) {
          string stereotype{"<<procedure>>"};
          string plugin{"No plugin name available"};
          if (step["plugin"] and step["plugin"]["name"]) {
            plugin = step["plugin"]["name"].as<string>();
            if (plugin == "fetch") {
              stereotype = "<<input>>";
            }
          }
          string description{"No description available"};
          if (step["description"]) {
            description = step["description"].as<string>();
          }

          ret += beginDiagram;
          ret += "partition \"Alternative flow\" {\n";
          ret += fmt::format(":{}; {}\n", step["name"].as<string>(), stereotype);
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
void MicroCI::RegisterPlugin(const string &name, shared_ptr<PluginStepParser> pluginStepParser) {
  mPluginParserMap[name] = pluginStepParser;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::Script() -> stringstream & {
  if (IsValid()) {
    return mScript;
  }
  throw runtime_error("Invalid configuration detected");
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultDockerImage() const -> string { return mDefaultDockerImage; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultWorkspace() const -> string { return mDefaultWorkspace; }

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
void MicroCI::SetAppendLog(const bool appendLog) { mAppendLog = appendLog; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ToString() const -> string {
  if (IsValid()) {
    return mScript.str();
  }
  throw runtime_error("Invalid configuration detected");
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::AddDockerImage(const string &image) { mDockerImages.insert(image); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::LoadEnvironmentFromYamlFile(const string &filename) {
  if (filesystem::exists(filename)) {
    auto dotEnv = YAML::LoadFile(filename);
    if (dotEnv.size() == 0) {
      spdlog::error("The file {} was found but it has no valid configuration", filename);
      invalidConfigurationDetected();
      // return false;
    }
    for (auto it : dotEnv) {
      EnvironmentVariable env;
      env.name = it.first.as<string>();
      env.value = it.second.as<string>();
      mEnvs.insert(env);
    }
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MicroCI::LoadEnvironmentFromEnvFile(const string &filename) {
  if (filesystem::exists(filename)) {
    ifstream env(filename);
    string line;
    while (getline(env, line)) {
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
      if (eqPos != string::npos) {
        EnvironmentVariable env;
        env.name = line.substr(0, eqPos);
        env.value = line.substr(eqPos + 1);
        mEnvs.insert(env);
      }
    }
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::ReadConfig(const string &filename) -> bool {
  YAML::Node CI;

  try {
    CI = YAML::LoadFile(filename);

    // Global configuration
    {
      struct passwd *pw = getpwuid(getuid());
      auto globalEnv = fmt::format("{}/.microCI.env", pw->pw_dir);
      LoadEnvironmentFromEnvFile(globalEnv);
    }

    // Start with global environment variables
    if (CI["envs"] and CI["envs"].IsMap()) {
      for (auto it : CI["envs"]) {
        EnvironmentVariable env;
        env.name = it.first.as<string>();
        env.value = it.second.as<string>();
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
    mDefaultDockerImage = CI["docker"].as<string>();
  }

  if (!mOnlyStep.empty()) {
    // FIXME: Verificar se existe
    for (auto step : CI["steps"]) {
      if (!step["plugin"] or !step["plugin"]["name"]) {
        throw invalid_argument(fmt::format("Plugin not defined at the step '{}'", step["name"].as<string>()));
      }
      if (step["only"] and step["only"].as<string>() == mOnlyStep) {
        parsePluginStep(step);
        mScript << "# Execute only this step" << endl;
        mScript << "step_" << sanitizeName(step["name"].as<string>()) << endl;
        mScript << "exit 0;" << endl;
      }
    }
  } else {
    if (CI["steps"].IsSequence()) {
      for (auto step : CI["steps"]) {
        if (!step["plugin"] or !step["plugin"]["name"]) {
          throw invalid_argument(fmt::format("Plugin not defined at the step '{}'", step["name"].as<string>()));
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
        mScript << "# Update docker images used in the steps\n";
        for (const auto &dockerImage : mDockerImages) {
          mScript << fmt::format("if docker image inspect {} > /dev/null 2>&1 ; then\n", dockerImage);
          mScript << fmt::format("  echo 'Docker image {} is already updated' >> .microCI.log\n", dockerImage);
          mScript << fmt::format("else\n");
          mScript << fmt::format("  echo 'Updating {} docker image...'\n", dockerImage);
          mScript << fmt::format("  docker pull {} 2>&1 >> .microCI.log\n", dockerImage);
          mScript << fmt::format("fi\n");
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
        auto stepName = step["name"].as<string>();
        mScript << "  step_" << sanitizeName(stepName) << endl;
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
  auto pluginName = step["plugin"]["name"].as<string>();

  if (mPluginParserMap.count(pluginName)) {
    mPluginParserMap.at(pluginName)->Parse(step);
    if (!mPluginParserMap.at(pluginName)->IsValid()) {
      invalidConfigurationDetected();
    }
    return;
  } else {
    auto stepName = step["name"].as<string>();
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
auto MicroCI::DefaultVolumes() const -> set<DockerVolume> {
  set<DockerVolume> volumes{{"/microci_workspace", "${MICROCI_PWD}", "rw"}};
  return volumes;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultEnvs() const -> set<EnvironmentVariable> { return mEnvs; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto MicroCI::DefaultDataTemplate() const -> json {
  json data;
  data["VERSION"] = fmt::format("v{}       ", microCI_version).substr(0, 10);
  data["WORKSPACE"] = mDefaultWorkspace;

  // Network docker: bridge (default), host, none
  data["DOCKER_NETWORK"] = "none";
  data["DOCKER_IMAGE"] = mDefaultDockerImage;
  data["RUN_AS"] = "root";
  data["MICROCI_STEP_SKIP"] = "no";
  data["MICROCI_YAML"] = mYamlFilename;

  data["BLUE"] = "\033[0;34m";
  data["YELLOW"] = "\033[0;33m";
  data["MAGENTA"] = "\033[0;35m";
  data["RED"] = "\033[0;31m";
  data["GREEN"] = "\033[0;32m";
  data["CYAN"] = "\033[0;36m";
  data["CLEAR"] = "\033[0m";

  data["APPEND_LOG_TEE_FLAG"] = mAppendLog ? " -a" : "";

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
        stepsComments << "# Step: " << step["name"].as<string>() << "\n";
      }
      if (step["description"]) {
        stepsComments << "#       " << step["description"].as<string>() << "\n\n";
      }
    }
  }
  data["STEPS_COMMENTS"] = stepsComments.str();

  auto scriptMicroCI = string{reinterpret_cast<const char *>(___sh_MicroCI_sh), ___sh_MicroCI_sh_len};
  mScript << inja::render(scriptMicroCI, data) << endl;

  auto envs = DefaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    auto scriptNotifyDiscord =
        string{reinterpret_cast<const char *>(___sh_NotifyDiscord_sh), ___sh_NotifyDiscord_sh_len};

    mScript << inja::render(scriptNotifyDiscord, data) << endl;
  } else {
    mScript << "# Notification by Discord is not possible" << endl;
  }
}

}  // namespace microci
