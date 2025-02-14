// MIT License
//
// Copyright (C) 2022-2025 Geraldo Luis da Silva Ribeiro
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

#ifndef PLUGIN_STEP_PARSER_HPP
#define PLUGIN_STEP_PARSER_HPP

#include <yaml-cpp/yaml.h>

#include <inja.hpp>
#include <nlohmann/json.hpp>
#include <string>

#include "MicroCI.hpp"
#include "MicroCIUtils.hpp"

namespace microci {
using namespace std;
using nlohmann::json;

class PluginStepParser {
 public:
  PluginStepParser(MicroCI *microCI) : mMicroCI(microCI) {}
  virtual ~PluginStepParser() {}
  virtual void Parse(const YAML::Node &step) { mMicroCI->Script() << step["name"].as<string>() << endl; }
  virtual bool IsValid() const;

 protected:
  void invalidConfigurationDetected();
  void beginFunction(const json &data, const set<EnvironmentVariable> &envs);
  void endFunction(const json &data);

  [[nodiscard]] json parseRunAs(const YAML::Node &step, const json &data, const string &defaultValue) const;
  [[nodiscard]] json parseNetwork(const YAML::Node &step, const json &data, const string &defaultValue) const;
  [[nodiscard]] json parseDevices(const YAML::Node &step, const json &data) const;
  [[nodiscard]] set<EnvironmentVariable> parseEnvs(const YAML::Node &step) const;
  [[nodiscard]] set<DockerVolume> parseVolumes(const YAML::Node &step) const;
  [[nodiscard]] tuple<json, set<DockerVolume>, set<EnvironmentVariable>> parseSsh(
      const YAML::Node &step, const json &data, const set<DockerVolume> &volumes,
      const set<EnvironmentVariable> &envs) const;

  void copySshIfAvailable(const YAML::Node &step, const json &data);

  [[nodiscard]] string stepDockerImage(const YAML::Node &step, const string &image = "") const;
  [[nodiscard]] string stepDockerWorkspace(const YAML::Node &step, const string &workspace = "") const;

  void prepareRunDocker(const json &data, const set<EnvironmentVariable> &envs, const set<DockerVolume> &volumes);

  MicroCI *mMicroCI = nullptr;
  bool mIsValid = true;
};

}  // namespace microci

#endif
