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

#include <PandocPluginStepParser.hpp>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PandocPluginStepParser::Parse(const YAML::Node &step) {
  auto output = string{"output.pdf"};
  auto basePath = string{"."};
  list<string> inputList;
  list<string> optionList = {"--pdf-engine=xelatex"};

  if (step["plugin"]["output"]) {
    output = step["plugin"]["output"].as<string>();
  }

  if (step["plugin"]["base_path"]) {
    basePath = step["plugin"]["base_path"].as<string>();
  }

  if (step["plugin"]["inputs"] && step["plugin"]["inputs"].IsSequence()) {
    for (const auto &filename : step["plugin"]["inputs"]) {
      inputList.push_back(filename.as<string>());
    }
  }

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto &opt : step["plugin"]["options"]) {
      optionList.push_back(opt.as<string>());
    }
  }

  auto data = mMicroCI->DefaultDataTemplate();
  data = parseNetwork(step, data, "host");

  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Create documentation using pandoc");
  data["DOCKER_IMAGE"] = stepDockerImage(step, "intmain/microci_pandoc:latest");
  data["BASE_PATH"] = basePath;

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

  // pdflatex is not designed for unicode, using xelatex instead
  mMicroCI->Script() << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --user $(id -u):$(id -g) \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_{{ FUNCTION_NAME }}_$(head -c 8 /proc/sys/kernel/random/uuid) \
        --workdir {{ WORKSPACE }}/{{BASE_PATH}} \
        --volume "${MICROCI_PWD}":{{ WORKSPACE }} \
        --network {{ DOCKER_NETWORK }} \
        {{ DOCKER_IMAGE }} \
)",
                                     data);
  for (const auto &option : optionList) {
    mMicroCI->Script() << "        " << option << " \\\n";
  }
  for (const auto &input : inputList) {
    mMicroCI->Script() << "        " << input << " \\\n";
  }
  mMicroCI->Script() << "        -o " << output << " 2>&1";
  endFunction(data);
}
}  // namespace microci
