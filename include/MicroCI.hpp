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
