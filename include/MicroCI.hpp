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
#include <string>

//

#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>

namespace microci {

using nlohmann::json;

#define microCI_version "0.44.0"

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct DockerVolume {
  std::string destination;
  std::string source;
  std::string mode;
};

// Using the target as key allowing to mount the same folder at more than one place.
inline bool operator<(const DockerVolume &lhs, const DockerVolume &rhs) { return lhs.destination < rhs.destination; }

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
inline bool operator<(const EnvironmentVariable &lhs, const EnvironmentVariable &rhs) { return lhs.name < rhs.name; }

class PluginStepParser;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
class MicroCI {
 public:
  MicroCI();
  virtual ~MicroCI();
  [[nodiscard]] std::string ToString() const;
  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] std::string ActivityDiagram(const std::string &filename) const;
  [[nodiscard]] std::string List(const std::string &filename) const;

  // Variável de ambiente global a todos os passos
  void SetEnvironmentVariable(const EnvironmentVariable &env);
  bool ReadConfig(const std::string &yaml);
  std::stringstream &Script();
  void SetAltHome(const std::string &altHome);
  void SetOnlyStep(const std::string &onlyStep);
  void SetOnlyStepNumber(const std::size_t number);
  void SetOnlyStepHash(const std::string &fileName, const std::string &hh);
  void SetAppendLog(const bool appendLog);
  void AddDockerImage(const std::string &image);

  [[nodiscard]] json DefaultDataTemplate() const;
  std::string DefaultDockerImage() const;
  std::string DefaultWorkspace() const;
  [[nodiscard]] std::set<DockerVolume> DefaultVolumes() const;
  [[nodiscard]] std::set<EnvironmentVariable> DefaultEnvs() const;
  void RegisterPlugin(const std::string &name, std::shared_ptr<microci::PluginStepParser> pluginStepParser);

 private:
  void initBash(const YAML::Node &CI);
  void parsePluginStep(const YAML::Node &step);
  void invalidConfigurationDetected();
  void LoadEnvironmentFromYamlFile(const std::string &filename);
  void LoadEnvironmentFromEnvFile(const std::string &filename);

  std::string mAltHome;
  std::string mYamlFilename = ".microCI.yml";
  std::set<EnvironmentVariable> mEnvs;
  std::set<std::string> mDockerImages;
  std::string mOnlyStep;
  std::optional<std::size_t> mOnlyStepNumber;
  std::string mDefaultDockerImage;
  std::string mDefaultWorkspace;
  std::stringstream mScript;
  bool mIsValid   = true;
  bool mAppendLog = false;

  std::map<std::string, std::shared_ptr<microci::PluginStepParser>> mPluginParserMap;
};

}  // namespace microci
#endif
