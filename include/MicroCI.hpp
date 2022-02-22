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
const int MINOR = 6;
const int PATCH = 0;

string banner();

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

class MicroCI {
 public:
  MicroCI();
  virtual ~MicroCI();
  bool ReadConfig(const string& yaml);
  string Script() const;
  void SetOnlyStep(const string& onlyStep);

 private:
  void initBash();
  void parseBashStep(YAML::Node& step);
  void parsePluginStep(YAML::Node& step);
  void parseGitDeployPluginStep(YAML::Node& step);
  void parseGitPublishPluginStep(YAML::Node& step);
  void parseMkdocsMaterialPluginStep(YAML::Node& step);
  void prepareRunDocker(const json& data, set<DockerVolume>& volumes);
  string stepRequiredValue(YAML::Node& step, const string& var) const;
  string stepOptionalValue(YAML::Node& step, const string& var,
                           const string& defaultValue) const;
  string stepDockerImage(YAML::Node& step, const string& image = "") const;
  string stepDescription(YAML::Node& step,
                         const string& defaultDescription = "") const;
  string stepName(YAML::Node& step) const;
  set<DockerVolume> parseVolumes(YAML::Node& step) const;

  json defaultDataTemplate() const;
  set<DockerVolume> defaultVolumes() const;

  string sanitizeName(const string& name) const;
  void beginFunction(const json& data);
  void endFunction(const json& data);

  string mOnlyStep;
  map<string, string> mEnvs;
  string mDockerImageGlobal;
  stringstream mScript;

  typedef void (MicroCI::*parseFunctionPtr)(YAML::Node& step);
  map<string, parseFunctionPtr> mPluginParserMap;
};

}  // namespace microci
#endif
