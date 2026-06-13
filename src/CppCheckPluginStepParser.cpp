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

#include "CppCheckPluginStepParser.hpp"

#include <fstream>
#include <spdlog/spdlog.h>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void CppCheckPluginStepParser::Parse(const YAML::Node &step) {
  auto platform = std::string{"unix64"};
  auto standard = std::string{"c++11"};
  std::list<std::string> includeList;
  std::list<std::string> sourceList;
  std::list<std::string> opts{"--enable=all", "--inconclusive", "--xml", "--xml-version=2"};

  auto data    = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs    = parseEnvs(step);
  data         = parseRunAs(step, data, "user");
  data         = parseNetwork(step, data, "none");

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto &opt : step["plugin"]["options"]) {
      opts.push_back(opt.as<std::string>());
    }
  }

  if (step["plugin"]["include"] && step["plugin"]["include"].IsSequence()) {
    for (const auto &inc : step["plugin"]["include"]) {
      includeList.push_back(inc.as<std::string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<std::string>());
    }
  }

  if (step["plugin"]["platform"]) {
    platform = step["plugin"]["platform"].as<std::string>();
  }

  if (step["plugin"]["std"]) {
    standard = step["plugin"]["std"].as<std::string>();
  }

  data["STEP_NAME"]        = stepName(step);
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Check C++ code");
  data["PLATFORM"]         = platform;
  data["STD"]              = standard;
  data["REPORT_TITLE"]     = "MicroCI::CppCheck";

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }} \
        && mkdir -p auditing/cppcheck \
        && rm -f auditing/cppcheck/*.html \
        && cppcheck \
          --platform={{ PLATFORM }} \
          --std={{ STD }} \
)",
                                     data);

  for (const auto &opt : opts) {
    mMicroCI->Script() << "          " << opt << " \\\n";
  }

  for (const auto &inc : includeList) {
    mMicroCI->Script() << "          --include=" << inc << " \\\n";
  }

  for (const auto &src : sourceList) {
    mMicroCI->Script() << "          " << src << " \\\n";
  }

  // XML is written to stderr
  mMicroCI->Script() << inja::render(R"(          2> auditing/cppcheck/cppcheck.xml \
        && cppcheck-htmlreport \
          --title='{{ REPORT_TITLE }}' \
          --report-dir='auditing/cppcheck/' \
          --source-dir='.' \
          --file='auditing/cppcheck/cppcheck.xml' 2>&1"
)",
                                     data);

  endFunction(data);
}
}  // namespace microci
