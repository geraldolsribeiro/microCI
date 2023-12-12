// MIT License
//
// Copyright (C) 2022-2023 Geraldo Luis da Silva Ribeiro
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

#include <BashPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void BashPluginStepParser::Parse(const YAML::Node &step) {
  auto cmdsStr = string{};
  auto cmds = vector<string>{};
  auto line = string{};

  auto data = mMicroCI->DefaultDataTemplate();

  if (step["plugin"]["bash"]) {
    cmdsStr = step["plugin"]["bash"].as<string>();
  } else if (step["plugin"]["sh"]) {
    cmdsStr = step["plugin"]["sh"].as<string>();
  } else {
    spdlog::error("Tratar erro aqui");
    throw invalid_argument("Script não encontrado");
  }

  auto ss = stringstream{cmdsStr};
  while (getline(ss, line, '\n')) {
    if (!line.empty() && line.at(0) != '#') {
      cmds.push_back(line);
    }
  }

  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  data["STEP_NAME"] = stepName(step);
  data["STEP_DESCRIPTION"] = stepDescription(step, "Executa comandos no bash");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"] = stepDockerImage(step);

  mMicroCI->Script() << "# bash \n";
  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  if (step["plugin"]["sh"]) {
    mMicroCI->Script() << inja::render(R"( \
        /bin/sh -c "cd {{ WORKSPACE }})",
                                       data);
  } else if (step["plugin"]["bash"]) {
    mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                       data);
  }

  copySshIfAvailable(step, data);

  for (auto cmd : cmds) {
    mMicroCI->Script() << fmt::format(" \\\n           && {} 2>&1", cmd);
  }
  mMicroCI->Script() << "\"\n";
  endFunction(data);
}
}  // namespace microci
