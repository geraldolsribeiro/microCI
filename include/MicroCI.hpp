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
#include <memory>
#include <set>
#include <string>

using namespace std;

#include <yaml-cpp/yaml.h>

#include <MicroCI.hpp>
#include <nlohmann/json.hpp>

namespace microci {

using nlohmann::json;

const int MAJOR = 0;
const int MINOR = 20;
const int PATCH = 0;

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

  // Variável de ambiente global a todos os passos
  void SetEnvironmentVariable(const EnvironmentVariable &env);
  bool ReadConfig(const string &yaml);
  stringstream &Script();
  void SetOnlyStep(const string &onlyStep);

  [[nodiscard]] json DefaultDataTemplate() const;
  string DefaultDockerImage() const;
  string DefaultWorkspace() const;
  [[nodiscard]] set<DockerVolume> DefaultVolumes() const;
  [[nodiscard]] set<EnvironmentVariable> DefaultEnvs() const;
  void RegisterPlugin(const string &name, shared_ptr<microci::PluginStepParser> pluginStepParser);

 private:
  void initBash();
  void parsePluginStep(const YAML::Node &step);

  set<EnvironmentVariable> mEnvs;
  string mOnlyStep;
  string mDefaultDockerImage;
  string mDefaultWorkspace;
  stringstream mScript;
  bool mIsValid = true;

  map<string, shared_ptr<microci::PluginStepParser>> mPluginParserMap;
};

}  // namespace microci
#endif
