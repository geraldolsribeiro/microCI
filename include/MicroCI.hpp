#ifndef MICRO_CI_HPP
#define MICRO_CI_HPP

#include <map>
#include <string>

using namespace std;

#include <yaml-cpp/yaml.h>

namespace microci {
const int MAJOR = 0;
const int MINOR = 0;
const int PATCH = 2;

string banner();

class MicroCI {
 public:
  MicroCI();
  virtual ~MicroCI();
  bool LoadYaml(const string& yaml);
  string Bash() const;

 private:
  void initBash();
  void parseBashStep(YAML::Node& step);
  void parsePluginStep(YAML::Node& step);
  string sanitizeName(const string& name) const;

  map<string, string> mEnvs;
  string mDockerImageGlobal;
  stringstream mBash;
};

}  // namespace microci
#endif
