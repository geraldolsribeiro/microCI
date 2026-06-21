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

#ifndef PLUGIN_STEP_PARSER_HPP
#define PLUGIN_STEP_PARSER_HPP

#include <fmt/core.h>
#include <string>
#include <yaml-cpp/yaml.h>

#include "3rd/inja.hpp"
#include "MicroCI.hpp"
#include "MicroCIUtils.hpp"

namespace microci {
using nlohmann::json;

class PluginStepParser {
 public:
  explicit PluginStepParser(MicroCI *microCI) : mMicroCI(microCI) {}
  virtual ~PluginStepParser() = default;
  virtual void Parse(const YAML::Node &step) { mMicroCI->Script() << step["name"].as<std::string>() << '\n'; }
  [[nodiscard]] virtual auto IsValid() const -> bool;

 protected:
  void invalidConfigurationDetected();
  void beginFunction(const json &data, const std::set<EnvironmentVariable> &envs);
  void endFunction(const json &data);

  [[nodiscard]] auto parseRunAs(const YAML::Node &step, const json &data, const std::string &defaultValue) const
      -> json;
  [[nodiscard]] auto parseNetwork(const YAML::Node &step, const json &data, const std::string &defaultValue) const
      -> json;
  [[nodiscard]] auto parseDevices(const YAML::Node &step, const json &data) const -> json;
  [[nodiscard]] auto parseEnvs(const YAML::Node &step) const -> std::set<EnvironmentVariable>;
  [[nodiscard]] auto parseVolumes(const YAML::Node &step) const -> std::set<DockerVolume>;
  [[nodiscard]] auto parseSsh(const YAML::Node &step, const json &data, const std::set<DockerVolume> &volumes,
                              const std::set<EnvironmentVariable> &envs) const
      -> std::tuple<json, std::set<DockerVolume>, std::set<EnvironmentVariable>>;

  void copySshIfAvailable(const YAML::Node &step, const json &data);

  [[nodiscard]] auto stepDockerImage(const YAML::Node &step, const std::string &overrideDefaultImage = "") const
      -> std::string;
  [[nodiscard]] auto stepDockerWorkspace(const YAML::Node &step, const std::string &workspace = "") const
      -> std::string;

  void prepareRunDocker(const json &data, const std::set<EnvironmentVariable> &envs,
                        const std::set<DockerVolume> &volumes);

  MicroCI *mMicroCI = nullptr;
  bool mIsValid     = true;
};

}  // namespace microci

#endif
