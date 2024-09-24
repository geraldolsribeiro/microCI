// MIT License
//
// Copyright (C) 2022-2024 Geraldo Luis da Silva Ribeiro
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

#include <DockerBuildPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void DockerBuildPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto envs = parseEnvs(step);
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Builds a docker image");

  auto folder = step["plugin"]["folder"].as<string>("dockerfiles");
  auto dockerfile = step["plugin"]["dockerfile"].as<string>("Dockerfile");
  auto targetName = step["plugin"]["target"].as<string>("my_image");
  auto targetVersion = step["plugin"]["version"].as<string>("0.1.0");
  auto isLatest = step["plugin"]["is_latest"].as<bool>(false);
  auto doPush = step["plugin"]["push"].as<bool>(false);

  data["FOLDER"] = folder;
  data["TARGET_NAME"] = targetName;
  data["VERSION"] = targetVersion;
  data["DOCKERFILE"] = dockerfile;

  beginFunction(data, envs);
  mMicroCI->Script() << inja::render(R"( \
        pushd {{ FOLDER }} \
          && docker build -t {{ TARGET_NAME }}:{{ VERSION }} -f {{ DOCKERFILE }} . 2>&1)",
                                     data);

  if (isLatest) {
    mMicroCI->Script() << inja::render(R"( \
          && docker tag {{ TARGET_NAME }}:{{ VERSION }} {{ TARGET_NAME }}:latest 2>&1)",
                                       data);
  }

  if (doPush) {
    mMicroCI->Script() << inja::render(R"( \
          && docker login 2>&1 \
          && docker push {{ TARGET_NAME }}:{{ VERSION }} 2>&1)",
                                       data);
    if (isLatest) {
      mMicroCI->Script() << inja::render(R"( \
          && docker push {{ TARGET_NAME }}:latest 2>&1)",
                                         data);
    }
  }
  mMicroCI->Script() << inja::render(R"( \
          && popd
)",
                                     data);
  endFunction(data);
}
}  // namespace microci
