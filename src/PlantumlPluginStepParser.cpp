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

#include <PlantumlPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PlantumlPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  list<string> sourceList;
  list<string> opts = {"-nometadata", "-charset utf-8 ", "-r"};

  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto &opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  auto type = step["plugin"]["type"].as<string>("png");
  auto outputFolder = step["plugin"]["output_folder"].as<string>("");
  auto config = step["plugin"]["config"].as<string>("");

  // para executar com GUI
  // -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/

  opts.push_back("-t" + type);

  if (!config.empty()) {
    opts.push_back("-config " + config);
  }

  if (!outputFolder.empty()) {
    opts.push_back("-o /microci_workspace/" + outputFolder);
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "intmain/microci_plantuml:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Build diagrams from textual description");
  data["OUTPUT"] = outputFolder;

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mMicroCI->Script() << inja::render(R"( \
          /bin/bash -c "cd {{ WORKSPACE }} \
          && export XDG_CACHE_HOME=$(mktemp -d) \
          && java -jar /usr/share/java/plantuml.jar \
)",
                                     data);

  for (const auto &opt : opts) {
    mMicroCI->Script() << "            " << opt << " \\\n";
  }

  for (const auto &src : sourceList) {
    mMicroCI->Script() << "            " << src << " \\\n";
  }

  mMicroCI->Script() << inja::render(
      R"(            2>&1"
)",
      data);

  endFunction(data);
}
}  // namespace microci
