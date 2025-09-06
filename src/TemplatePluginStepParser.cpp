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

#include "TemplatePluginStepParser.hpp"

#include <spdlog/spdlog.h>

#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void TemplatePluginStepParser::Parse(const YAML::Node &step) {
  auto data    = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs    = parseEnvs(step);
  // bool toc = false;
  // bool details = false;
  // bool show_source = false;
  // bool show_banner = true;

  EnvironmentVariable env;
  env.name  = "RUST_BACKTRACE";
  env.value = "full";
  envs.insert(env);

  // if (step["plugin"]["toc"]) {
  //   toc = step["plugin"]["toc"].as<bool>();
  // }
  // env.name = "TEMPLATE_TOC";
  // env.value = toc ? "true" : "false";
  // envs.insert(env);

  // if (step["plugin"]["details"]) {
  //   details = step["plugin"]["details"].as<bool>();
  // }
  // env.name = "TEMPLATE_DETAILS";
  // env.value = details ? "true" : "false";
  // envs.insert(env);

  // if (step["plugin"]["show_source"]) {
  //   show_source = step["plugin"]["show_source"].as<bool>();
  // }
  // env.name = "TEMPLATE_SHOW_SOURCE";
  // env.value = show_source ? "true" : "false";
  // envs.insert(env);

  // if (step["plugin"]["show_banner"]) {
  //   show_banner = step["plugin"]["show_banner"].as<bool>();
  // }
  // env.name = "TEMPLATE_SHOW_BANNER";
  // env.value = show_banner ? "true" : "false";
  // envs.insert(env);

  data["STEP_NAME"]        = stepName(step);
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "chevdor/tera:latest");
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Generate output from template");

  beginFunction(data, envs);

  if (step["plugin"]["items"] && step["plugin"]["items"].IsSequence()) {
    for (const auto &item : step["plugin"]["items"]) {
      if (item.IsSequence()) {
        data["TEMPLATE_DATA"]   = item[0].as<string>();
        data["TEMPLATE_INPUT"]  = item[1].as<string>();
        data["TEMPLATE_OUTPUT"] = item[2].as<string>();

        prepareRunDocker(data, envs, volumes);
        mMicroCI->Script() << inja::render(R"( --template /microci_workspace/{{ TEMPLATE_INPUT }} \
        /microci_workspace/{{ TEMPLATE_DATA }} \
        > {{ TEMPLATE_OUTPUT }})",
                                           data);
        // Option --out does not work here
      }
    }
  }

  endFunction(data);
}
}  // namespace microci
