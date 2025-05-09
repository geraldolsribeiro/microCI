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

#include <DoxygenPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void DoxygenPluginStepParser::Parse(const YAML::Node &step) {
  auto header = string{};
  auto footer = string{};
  auto stylesheet = string{};
  auto doxyfile = string{"./Doxyfile"};
  auto output_dir = string{"doxygen"};

  if (step["plugin"]["html"]) {
    if (step["plugin"]["html"]["header"]) {
      header = step["plugin"]["html"]["header"].as<string>();
    }
    if (step["plugin"]["html"]["footer"]) {
      footer = step["plugin"]["html"]["footer"].as<string>();
    }
    if (step["plugin"]["html"]["stylesheet"]) {
      stylesheet = step["html"]["stylesheet"].as<string>();
    }
  }

  if (step["plugin"]["doxyfile"]) {
    doxyfile = step["plugin"]["doxyfile"].as<string>();
  }

  if (step["plugin"]["output_dir"]) {
    output_dir = step["plugin"]["output_dir"].as<string>();
  }

  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  data["STEP_NAME"] = stepName(step);
  data["STEP_DESCRIPTION"] = stepDescription(step, "Execute commands at bash shell");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"] = stepDockerImage(step);
  data["DOXYFILE"] = doxyfile;
  data["HEADER"] = header;
  data["FOOTER"] = footer;
  data["STYLESHEET"] = stylesheet;
  data["OUTPUT_DIRECTORY"] = output_dir;

  mMicroCI->Script() << "# bash \n";
  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mMicroCI->Script() << inja::render(R"( bash -c "cd {{ WORKSPACE }} \
          && if [ ! -f {{ DOXYFILE }} ]; then doxygen -g {{ DOXYFILE }}; fi \
          && doxygen -u {{ DOXYFILE }} \
          && sed -i 's#^OUTPUT_DIRECTORY.*#OUTPUT_DIRECTORY = {{ OUTPUT_DIRECTORY }}#' {{ DOXYFILE }} \
          && sed -i 's#^HAVE_DOT.*#HAVE_DOT = YES#' {{ DOXYFILE }} \
          && sed -i 's#^CALL_GRAPH.*#CALL_GRAPH = YES#' {{ DOXYFILE }} \
          && sed -i 's#^CALLER_GRAPH.*#CALLER_GRAPH = YES#' {{ DOXYFILE }} \
          && sed -i 's#^WARN_LOGFILE.*#WARN_LOGFILE = {{ OUTPUT_DIRECTORY }}/doxygen.log#' {{ DOXYFILE }} \
          && mkdir -p auditing \
          && doxygen {{ DOXYFILE }}; \
          touch {{ OUTPUT_DIRECTORY }}/doxygen.log \
          && sed -i 's#/microci_workspace/##g' {{ OUTPUT_DIRECTORY }}/doxygen.log"
)",
                                     data);

  endFunction(data);
}
}  // namespace microci
