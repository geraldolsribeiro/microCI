// ░░░░░░░░░░░░░░░░░ microCI
// ░░░░░░░█▀▀░▀█▀░░░
// ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
// ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// ░░░▀░░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░ Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef MICRO_CI_HPP
#define MICRO_CI_HPP

#include <map>
#include <string>

using namespace std;

#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>

namespace microci {
using nlohmann::json;

const int MAJOR = 0;
const int MINOR = 3;
const int PATCH = 0;

string banner();

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
  void parseMkdocsMaterialPluginStep(YAML::Node& step);
  json defaultDataTemplate() const;
  string sanitizeName(const string& name) const;

  string mOnlyStep;
  map<string, string> mEnvs;
  string mDockerImageGlobal;
  stringstream mScript;

  typedef void (MicroCI::*parseFunctionPtr)(YAML::Node& step);
  map<string, parseFunctionPtr> mPluginParserMap;
};

}  // namespace microci
#endif
