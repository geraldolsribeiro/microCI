// MIT License
//
// Copyright (C) 2022-2024 Geraldo Luis da Silva Ribeiro
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

#include <ClangFormatPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void ClangFormatPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  list<string> sourceList;

  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "intmain/microci_cpp_compiler:latest");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "C++ code formatting");

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                     data);
  for (const auto &src : sourceList) {
    mMicroCI->Script() << fmt::format(R"( \
        && cat <(compgen -G '{}') \
          | )",
                                      src);
    mMicroCI->Script() << R"(xargs -I {} clang-format -i {} 2>&1 )";
  }

  mMicroCI->Script() << R"("
)";
  endFunction(data);
}
}  // namespace microci
