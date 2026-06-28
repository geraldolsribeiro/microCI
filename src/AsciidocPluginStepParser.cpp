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

#include "AsciidocPluginStepParser.hpp"

#include <algorithm>
#include <iterator>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void AsciidocPluginStepParser::Parse(const YAML::Node &step) {
  auto inputList      = std::list<std::string>{};
  auto outputFormat   = std::string{"html"};
  auto outputFile     = std::string{};
  auto destinationDir = std::string{};
  auto basePath       = std::string{"."};

  if (step["plugin"]["output"]) {
    outputFile = step["plugin"]["output"].as<std::string>();
  }

  if (step["plugin"]["output_format"]) {
    outputFormat = step["plugin"]["output_format"].as<std::string>();
  }

  if (step["plugin"]["destination_dir"]) {
    destinationDir = step["plugin"]["destination_dir"].as<std::string>();
  }

  if (step["plugin"]["input"] && step["plugin"]["input"].IsSequence()) {
    std::transform(step["plugin"]["input"].begin(), step["plugin"]["input"].end(), std::back_inserter(inputList),
                   [](const auto &filename) { return filename.template as<std::string>(); });
  }

  auto data = mMicroCI->DefaultDataTemplate();
  data      = parseNetwork(step, data, "host");

  data["STEP_NAME"]        = stepName(step);
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Convert AsciiDoc documentation");
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_asciidoc:latest");
  data["BASE_PATH"]        = basePath;
  data["OUTPUT_FORMAT"]    = outputFormat;
  data["DESTINATION_DIR"]  = destinationDir;
  data["OUTPUT_FILE"]      = outputFile;

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
        --name microci_{{ FUNCTION_NAME }}_{{ RANDOM_8 }} \
        --workdir {{ WORKSPACE }}/{{BASE_PATH}} \
        --volume "${MICROCI_PWD}":{{ WORKSPACE }} \
        --network {{ DOCKER_NETWORK }} \
        {{ DOCKER_IMAGE }} \
)",
                                     data);

  if (outputFormat == "pdf") {
    mMicroCI->Script() << "        asciidoctor-pdf \\\n";
  } else {
    mMicroCI->Script() << "        asciidoctor \\\n";
  }

  if (not destinationDir.empty()) {
    mMicroCI->Script() << inja::render(R"(          --destination-dir {{DESTINATION_DIR}} \
)",
                                       data);
  }

  if (not outputFile.empty()) {
    mMicroCI->Script() << inja::render(R"(          --out-file {{OUTPUT_FILE}} \
)",
                                       data);
  }

  for (const auto &input : inputList) {
    mMicroCI->Script() << "          " << input << " \\\n";
  }

  endFunction(data);
}
}  // namespace microci
