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

#include "ClangTidyPluginStepParser.hpp"

#include <spdlog/spdlog.h>

#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void ClangTidyPluginStepParser::Parse(const YAML::Node &step) {
  auto data    = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs    = parseEnvs(step);
  auto runAs   = string{};
  list<string> checkList;
  list<string> includeList;
  list<string> systemIncludeList;
  list<string> sourceList;
  list<string> optionList;

  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");

  bool fixSourceCode = step["plugin"]["fix"] and step["plugin"]["fix"].as<bool>(false);

  if (step["plugin"]["checks"] && step["plugin"]["checks"].IsSequence()) {
    for (const auto &inc : step["plugin"]["checks"]) {
      checkList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["options"] && step["plugin"]["options"].IsSequence()) {
    for (const auto &opt : step["plugin"]["options"]) {
      optionList.push_back(opt.as<string>());
    }
  }

  if (step["plugin"]["include"] && step["plugin"]["include"].IsSequence()) {
    for (const auto &inc : step["plugin"]["include"]) {
      includeList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["system_include"] && step["plugin"]["system_include"].IsSequence()) {
    for (const auto &inc : step["plugin"]["system_include"]) {
      systemIncludeList.push_back(inc.as<string>());
    }
  }

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  data["STEP_NAME"]        = stepName(step);
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "C++ code check with clang-tidy");

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }} \
        && mkdir -p auditing/clang-tidy/ \
        && date > auditing/clang-tidy/clang-tidy.log \
        && clang-tidy \
)",
                                     data);
  if (fixSourceCode) {
    mMicroCI->Script() << "        --fix \\\n";
    mMicroCI->Script() << "        --fix-errors \\\n";
  }

  for (const auto &inc : systemIncludeList) {
    mMicroCI->Script() << "        --system-headers " << inc << " \\\n";
  }

  if (checkList.empty()) {
    mMicroCI->Script() << "        -checks='-*,cppcoreguidelines-*' \\\n";
  } else {
    string concatenatedList;
    for (const auto &check : checkList) {
      if (!concatenatedList.empty()) {
        concatenatedList += ",";
      }
      concatenatedList += check;
    }
    mMicroCI->Script() << "        -checks='" << concatenatedList << "' \\\n";
  }

  for (const auto &src : sourceList) {
    mMicroCI->Script() << "        " << src << " \\\n";
  }

  if (!optionList.empty() or !includeList.empty()) {
    mMicroCI->Script() << "        -- \\\n";
  }

  for (const auto &opt : optionList) {
    mMicroCI->Script() << "        " << opt << " \\\n";
  }

  for (const auto &inc : includeList) {
    mMicroCI->Script() << "        -I" << inc << " \\\n";
  }

  mMicroCI->Script() << inja::render(
      R"(        2>&1 | tee auditing/clang-tidy/clang-tidy.log 2>&1 ; \
        clang-tidy-html auditing/clang-tidy/clang-tidy.log --out auditing/clang-tidy/index.html 2>&1"
)",
      data);

  endFunction(data);
}
}  // namespace microci
