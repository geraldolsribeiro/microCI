// MIT License
//
// Copyright (c) 2022-2026 Geraldo Luis da Silva Ribeiro
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

#ifndef MICRO_CI_HPP
#define MICRO_CI_HPP

#include <memory>
#include <optional>
#include <set>
#include <string>

//

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

#include "ConsoleBox.hpp"

namespace microci {

using nlohmann::json;

#define microCI_version "0.47.0"

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct DockerVolume {
  std::string destination;
  std::string source;
  std::string mode;
};

// Using the target as key allowing to mount the same folder at more than one place.
inline auto operator<(const DockerVolume &lhs, const DockerVolume &rhs) -> bool {
  return lhs.destination < rhs.destination;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct EnvironmentVariable {
  std::string name;
  std::string value;
};

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
inline auto operator<(const EnvironmentVariable &lhs, const EnvironmentVariable &rhs) -> bool {
  return lhs.name < rhs.name;
}

class PluginStepParser;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
class MicroCI {
 public:
  MicroCI();
  virtual ~MicroCI();
  [[nodiscard]] auto ToString() const -> std::string;
  [[nodiscard]] auto IsValid() const -> bool;
  [[nodiscard]] auto ActivityDiagram(const std::string &fileName) const -> std::string;
  [[nodiscard]] auto List(const std::string &fileName) const -> std::string;

  // Global environment variable available to all steps
  void SetEnvironmentVariable(const EnvironmentVariable &env);
  auto ReadConfig(const std::string &fileName) -> bool;
  auto Script() -> std::stringstream &;
  void SetAltHome(const std::string &altHome);
  void SetOnlyStep(const std::string &onlyStep);
  void SetOnlyStepNumber(const std::set<std::size_t> &onlyStepNumbers);
  void SetOnlyStepHash(const std::string &fileName, const std::string &hh);
  void SetAppendLog(const bool appendLog);
  void AddDockerImage(const std::string &image);

  [[nodiscard]] auto DefaultDataTemplate() const -> json;
  auto DefaultDockerImage() const -> const std::string &;
  auto DefaultWorkspace() const -> const std::string &;
  [[nodiscard]] static auto DefaultVolumes() -> std::set<DockerVolume>;
  [[nodiscard]] auto DefaultEnvs() const -> const std::set<EnvironmentVariable> &;
  void RegisterPlugin(const std::string &name, std::shared_ptr<microci::PluginStepParser> pluginStepParser);

 private:
  void initBash(const YAML::Node &CI);
  void parsePluginStep(const YAML::Node &step);
  void invalidConfigurationDetected();
  void LoadEnvironmentFromYamlFile(const std::string &fileName);
  void LoadEnvironmentFromEnvFile(const std::string &fileName);

  std::string mAltHome;
  std::string mYamlFilename = ".microCI.yml";
  std::set<EnvironmentVariable> mEnvs;
  std::set<std::string> mDockerImages;
  std::string mOnlyStep;
  std::set<std::size_t> mOnlyStepNumbers;
  std::string mDefaultDockerImage;
  std::string mDefaultWorkspace;
  std::stringstream mScript;
  bool mIsValid   = true;
  bool mAppendLog = false;

  std::map<std::string, std::shared_ptr<microci::PluginStepParser>> mPluginParserMap;
};

}  // namespace microci
#endif
