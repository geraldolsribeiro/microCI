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

#include <spdlog/spdlog.h>

#include "MermaidPluginStepParser.hpp"

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MermaidPluginStepParser::Parse(const YAML::Node &step) {
  std::string outputFormat{"png"};
  std::string outputFolder{"./"};
  list<string> inputList;
  list<string> optionList;

  if (step["plugin"]["output_format"]) {
    outputFormat = step["plugin"]["output_format"].as<std::string>();
  }

  if (step["plugin"]["output_folder"]) {
    outputFolder = step["plugin"]["output_folder"].as<std::string>();
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
  data      = parseNetwork(step, data, "host");

  data["STEP_NAME"]        = stepName(step);
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Create diagram using mermaid");
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_mermaid:latest");

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

  for( const auto &input: inputList ) {
    mMicroCI->Script() << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --user $(id -u):$(id -g) \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_{{ FUNCTION_NAME }}_{{ RANDOM_8 }} \
        --workdir {{ WORKSPACE }} \
        --volume "${MICROCI_PWD}":{{ WORKSPACE }} \
        --network {{ DOCKER_NETWORK }} \
        {{ DOCKER_IMAGE }} \
)",
                                     data);
    for (const auto &option : optionList) {
      mMicroCI->Script() << "        " << option << " \\\n";
    }

    std::filesystem::path outputFolderPath {outputFolder};
    std::filesystem::path inputPath{ input };
    auto outputPath = outputFolderPath / (inputPath.stem().string() + "." + outputFormat);

    mMicroCI->Script() << "        --input " << input << " \\\n";
    mMicroCI->Script() << "        --output " << outputPath.string() << " 2>&1\n";
  }
  endFunction(data);
}
}  // namespace microci
