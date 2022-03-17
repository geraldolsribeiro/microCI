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

#ifndef MICRO_CI_HPP
#define MICRO_CI_HPP

#include <map>
#include <set>
#include <string>

using namespace std;

#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>

namespace microci {
using nlohmann::json;

const int MAJOR = 0;
const int MINOR = 15;
const int PATCH = 1;

string banner();

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
struct DockerVolume {
  string destination;
  string source;
  string mode;
};

// Usando o destino como chave para permitir montar a mesma pasta em mais
// de um local
inline bool operator<(const DockerVolume& lhs, const DockerVolume& rhs) {
  return lhs.destination < rhs.destination;
}

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
inline bool operator<(const EnvironmentVariable& lhs,
                      const EnvironmentVariable& rhs) {
  return lhs.name < rhs.name;
}

// struct PluginStepParser {
//   void parse(const YAML::Node& step, stringstream& ss) = 0;
// };

class MicroCI {
 public:
  MicroCI();
  virtual ~MicroCI();
  // Variável de ambiente global a todos os passos
  void SetEnvironmentVariable(EnvironmentVariable& env);
  bool ReadConfig(const string& yaml);
  string Script() const;
  void SetOnlyStep(const string& onlyStep);

 private:
  void initBash();
  void parseBashStep(const YAML::Node& step);
  void parsePluginStep(const YAML::Node& step);
  void parseBeamerPluginStep(const YAML::Node& step);
  void parseFetchPluginStep(const YAML::Node& step);
  void parseGitDeployPluginStep(const YAML::Node& step);
  void parseGitPublishPluginStep(const YAML::Node& step);
  void parseCppCheckPluginStep(const YAML::Node& step);
  void parseMkdocsMaterialPluginStep(const YAML::Node& step);
  void parsePlantumlPluginStep(const YAML::Node& step);
  void parseClangTidyPluginStep(const YAML::Node& step);
  void parseClangFormatPluginStep(const YAML::Node& step);
  void prepareRunDocker(const json& data, const set<EnvironmentVariable>& envs,
                        const set<DockerVolume>& volumes);
  tuple<json, set<DockerVolume>, set<EnvironmentVariable>> parseSsh(
      const YAML::Node& step, const json& data,
      const set<DockerVolume>& volumes,
      const set<EnvironmentVariable>& envs) const;
  void copySshIfAvailable(const YAML::Node& step, const json& data);
  string stepRequiredValue(const YAML::Node& step, const string& var) const;
  string stepOptionalValue(const YAML::Node& step, const string& var,
                           const string& defaultValue) const;
  string stepDockerImage(const YAML::Node& step,
                         const string& image = "") const;
  string stepDockerWorkspace(const YAML::Node& step,
                             const string& workspace = "") const;
  string stepDescription(const YAML::Node& step,
                         const string& defaultDescription = "") const;
  string stepName(const YAML::Node& step) const;
  set<DockerVolume> parseVolumes(const YAML::Node& step) const;
  set<EnvironmentVariable> parseEnvs(const YAML::Node& step) const;
  json parseRunAs(const YAML::Node& step, const json& data) const;
  json parseNetwork(const YAML::Node& step, const json& data) const;
  json defaultDataTemplate() const;
  set<DockerVolume> defaultVolumes() const;
  set<EnvironmentVariable> defaultEnvs() const;

  string sanitizeName(const string& name) const;
  void beginFunction(const json& data, const set<EnvironmentVariable>& envs);
  void endFunction(const json& data);

  set<EnvironmentVariable> mEnvs;
  string mOnlyStep;
  string mDockerImageGlobal;
  string mDockerWorkspaceGlobal;
  stringstream mScript;

  typedef void (MicroCI::*parseFunctionPtr)(const YAML::Node& step);
  map<string, parseFunctionPtr> mPluginParserMap;
};

}  // namespace microci
#endif
