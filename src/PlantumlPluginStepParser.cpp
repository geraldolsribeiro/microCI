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

#include <PlantumlPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PlantumlPluginStepParser::Parse(const YAML::Node& step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  auto runAs = string{};
  auto type = string{"png"};
  auto output = string{};
  list<string> sourceList;
  list<string> opts = {"-r"};

  data = parseRunAs(step, data);
  data = parseNetwork(step, data);

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto& opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto& src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  if (step["plugin"]["type"]) {
    type = step["plugin"]["type"].as<string>();
  }

  if (step["plugin"]["output"]) {
    output = step["plugin"]["output"].as<string>();
  }

  // para executar com GUI
  // -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/

  opts.push_back("-t" + type);
  if (!output.empty()) {
    opts.push_back("-o " + output);
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] =
      stepDockerImage(step, "intmain/microci_plantuml:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Constroi diagramas plantuml");
  data["OUTPUT"] = output;

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mMicroCI->Script() << inja::render(R"( \
          /bin/bash -c "cd {{ WORKSPACE }} \
          && java -jar /opt/plantuml/plantuml.jar \
)",
                                     data);

  for (const auto& opt : opts) {
    mMicroCI->Script() << "            " << opt << " \\\n";
  }

  for (const auto& src : sourceList) {
    mMicroCI->Script() << "            " << src << " \\\n";
  }

  mMicroCI->Script() << inja::render(
      R"(            2>&1"
)",
      data);

  endFunction(data);
}  // namespace microci
}  // namespace microci
