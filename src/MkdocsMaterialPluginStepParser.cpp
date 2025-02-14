// MIT License
//
// Copyright (C) 2022-2025 Geraldo Luis da Silva Ribeiro
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

#include <MkdocsMaterialPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MkdocsMaterialPluginStepParser::Parse(const YAML::Node &step) {
  auto action = string{"build"};
  auto port = string{"8000"};

  if (step["plugin"]["action"]) {
    action = step["plugin"]["action"].as<string>();
    if (action == "serve") {
      action += " --dev-addr=0.0.0.0:8000";
    }
  }
  if (step["plugin"]["port"]) {
    port = step["plugin"]["port"].as<string>();
  }

  auto data = mMicroCI->DefaultDataTemplate();
  data = parseNetwork(step, data, "host");

  data["ACTION"] = action;
  data["PORT"] = port;
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Documentação usando mkdocs_material");
  data["DOCKER_IMAGE"] = "intmain/microci_mkdocs_material:0.4";

  // https://unix.stackexchange.com/questions/155551/how-to-debug-a-bash-script
  // exec 5> >(logger -t $0)
  // BASH_XTRACEFD="5"
  // PS4='$LINENO: '
  // set -x
  //
  // # Place your code here

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

  mMicroCI->Script() << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --user $(id -u):$(id -g) \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_{{ FUNCTION_NAME }} \
        --workdir {{ WORKSPACE }} \
        --volume "${MICROCI_PWD}":{{ WORKSPACE }} \
        --network {{ DOCKER_NETWORK }} \
        --publish {{PORT}}:8000 \
        {{ DOCKER_IMAGE }} \
        mkdocs {{ ACTION }} 2>&1
)",
                                     data);

  endFunction(data);
}
}  // namespace microci
