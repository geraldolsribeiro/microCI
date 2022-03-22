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

#include <spdlog/spdlog.h>

#include <GitPublishPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void GitPublishPluginStepParser::Parse(const YAML::Node& step) {
  auto pluginCopyTo =
      string{"/tmp/microci_deploy"};  // local onde user pode escrever
  auto pluginCopyFrom = string{"site"};
  auto cleanBefore = true;
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);

  data["DOCKER_NETWORK"] = "bridge";
  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  const auto name = step["plugin"]["name"].as<string>();
  const auto gitURL = step["plugin"]["git_url"].as<string>();

  if (step["plugin"]["copy_from"]) {
    pluginCopyFrom = step["plugin"]["copy_from"].as<string>();
  }

  if (step["plugin"]["copy_to"]) {
    pluginCopyTo = step["plugin"]["copy_to"].as<string>();
  }

  if (step["plugin"]["clean_before"]) {
    cleanBefore = step["plugin"]["clean_before"].as<bool>();
  }

  data["GIT_URL"] = gitURL;
  data["PLUGIN_COPY_TO"] = pluginCopyTo;
  data["PLUGIN_COPY_FROM"] = pluginCopyFrom;
  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "bitnami/git:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Publica arquivos em um repositório git");

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                     data);

  copySshIfAvailable(step, data);

  mMicroCI->Script() << inja::render(R"( \
           && git clone '{{ GIT_URL }}' --depth 1 '{{ PLUGIN_COPY_TO }}' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} config user.name  '$(git config --get user.name)' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} config user.email '$(git config --get user.email)' 2>&1 \)",
                                     data);

  if (cleanBefore) {
    mMicroCI->Script() << inja::render(R"(
           && git -C {{ PLUGIN_COPY_TO }} rm '*' 2>&1 \)",
                                       data);
  }

  mMicroCI->Script() << inja::render(R"(
           && cp -rv {{ PLUGIN_COPY_FROM }}/* {{ PLUGIN_COPY_TO }}/ 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} add . 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} commit -am ':rocket:microCI git_publish' 2>&1 \
           && git -C {{ PLUGIN_COPY_TO }} push origin master 2>&1 \
           && chown $(id -u):$(id -g) -Rv {{ PLUGIN_COPY_FROM }} 2>&1
  ")",
                                     data);

  endFunction(data);
}  // namespace microci
}  // namespace microci
