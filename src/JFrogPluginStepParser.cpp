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

#include "JFrogPluginStepParser.hpp"

#include <spdlog/spdlog.h>

#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void JFrogPluginStepParser::Parse(const YAML::Node &step) {
  auto data    = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs    = parseEnvs(step);
  auto cmdsStr = string{};
  auto cmds    = vector<string>{};
  auto line    = string{};

  data = parseRunAs(step, data, "user");

  if (step["plugin"]["bash"]) {
    cmdsStr = step["plugin"]["bash"].as<string>();
  } else if (step["plugin"]["sh"]) {
    cmdsStr = step["plugin"]["sh"].as<string>();
  }

  auto ss = stringstream{cmdsStr};
  while (getline(ss, line, '\n')) {
    if (!line.empty() && line.at(0) != '#') {
      cmds.push_back(line);
    }
  }

  for (const auto &envName : {"JFROG_ACCESS_TOKEN", "JFROG_URL"}) {
    auto it = envs.find(EnvironmentVariable{envName, ""});
    if (it == envs.end()) {
      spdlog::error("The environment variable called '{}' was not found", envName);
      invalidConfigurationDetected();
      return;
    } else {
      data[envName] = it->value;
    }
  }

  data["STEP_NAME"]         = stepName(step);
  data["DOCKER_IMAGE"]      = stepDockerImage(step, "releases-docker.jfrog.io/jfrog/jfrog-cli-full-v2-jf");
  data["FUNCTION_NAME"]     = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"]  = stepDescription(step, "Send files from/to the JFrog artifact manager");
  data["DOCKER_ENTRYPOINT"] = "";  // removes default entrypoint
  data["DOCKER_NETWORK"]    = "host";

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  // FIXME: Allow custom names
  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                     data);
  for (auto cmd : cmds) {
    mMicroCI->Script() << fmt::format(" \\\n           && {} --url=$JFROG_URL --access-token=$JFROG_ACCESS_TOKEN 2>&1",
                                      cmd);
  }
  mMicroCI->Script() << R"("
)";
  endFunction(data);
}
}  // namespace microci
