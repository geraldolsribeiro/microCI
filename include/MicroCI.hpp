// MIT License
//
// Copyright (c) 2022-2025 Geraldo Luis da Silva Ribeiro
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

using namespace std;

#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>

namespace microci {

using nlohmann::json;

#define microCI_version "0.41.0"

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct DockerVolume {
  string destination;
  string source;
  string mode;
};

// Using the target as key allowing to mount the same folder at more than one place.
inline bool operator<(const DockerVolume &lhs, const DockerVolume &rhs) { return lhs.destination < rhs.destination; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct EnvironmentVariable {
  string name;
  string value;
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
  [[nodiscard]] string ToString() const;
  [[nodiscard]] bool IsValid() const;
  [[nodiscard]] string ActivityDiagram(const string &filename) const;
  [[nodiscard]] string List(const string &filename) const;

  // Variável de ambiente global a todos os passos
  void SetEnvironmentVariable(const EnvironmentVariable &env);
  bool ReadConfig(const string &yaml);
  stringstream &Script();
  void SetAltHome(const string &altHome);
  void SetOnlyStep(const string &onlyStep);
  void SetOnlyStepNumber(const std::size_t number);
  void SetOnlyStepHash(const std::string &fileName, const std::string &hh);
  void SetAppendLog(const bool appendLog);
  void AddDockerImage(const string &image);

  [[nodiscard]] json DefaultDataTemplate() const;
  string DefaultDockerImage() const;
  string DefaultWorkspace() const;
  [[nodiscard]] set<DockerVolume> DefaultVolumes() const;
  [[nodiscard]] set<EnvironmentVariable> DefaultEnvs() const;
  void RegisterPlugin(const string &name, shared_ptr<microci::PluginStepParser> pluginStepParser);

 private:
  void initBash(const YAML::Node &CI);
  void parsePluginStep(const YAML::Node &step);
  void invalidConfigurationDetected();
  void LoadEnvironmentFromYamlFile(const string &filename);
  void LoadEnvironmentFromEnvFile(const string &filename);

  string mAltHome;
  string mYamlFilename = ".microCI.yml";
  set<EnvironmentVariable> mEnvs;
  set<string> mDockerImages;
  string mOnlyStep;
  std::optional<std::size_t> mOnlyStepNumber;
  string mDefaultDockerImage;
  string mDefaultWorkspace;
  stringstream mScript;
  bool mIsValid   = true;
  bool mAppendLog = false;

  map<string, shared_ptr<microci::PluginStepParser>> mPluginParserMap;
};

}  // namespace microci
#endif
