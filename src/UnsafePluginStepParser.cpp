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

#include "UnsafePluginStepParser.hpp"

#include <algorithm>
#include <fstream>
#include <iterator>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void UnsafePluginStepParser::Parse(const YAML::Node &step) {
  auto cmdsStr = std::string{};
  auto cmds    = std::vector<std::string>{};
  auto line    = std::string{};
  std::list<std::string> opts{};

  auto data = mMicroCI->DefaultDataTemplate();

  if (step["plugin"]["bash"]) {
    cmdsStr = step["plugin"]["bash"].as<std::string>();
  } else if (step["plugin"]["sh"]) {
    cmdsStr = step["plugin"]["sh"].as<std::string>();
  } else {
    errorConsoleBox({fmt::format("No 'bash' or 'sh' script defined in plugin configuration")});
    invalidConfigurationDetected();
    throw std::invalid_argument("Script not found");
  }

  auto ss = std::stringstream{cmdsStr};
  while (getline(ss, line, '\n')) {
    if (!line.empty() && line.at(0) != '#') {
      cmds.push_back(line);
    }
  }

  for (const auto name : {"docker", "network", "volumes", "run_as", "devices", "ssh"}) {
    if (step[name]) {
      throw std::invalid_argument(fmt::format("unsafe plugin does not support the '{}' field", name));
    }
  }

  auto envs                = parseEnvs(step);
  data["STEP_NAME"]        = stepName(step);
  data["STEP_DESCRIPTION"] = stepDescription(step, "Execute commands at bash shell");
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_NAME_PREFIX"] = "🔴🔴🔴";

  mMicroCI->Script() << "# 🔴🔴🔴 UNSAFE STEP BEGIN 🔴🔴🔴\n";

  if (step["plugin"]["required"]) {
    if (step["plugin"]["required"]["commands"] && step["plugin"]["required"]["commands"].IsSequence()) {
      for (auto comm : step["plugin"]["required"]["commands"]) {
        data["COMMAND"]          = comm.as<std::string>();
        data["COMMAND_CENTERED"] = fmt::format("{:^52}", comm.as<std::string>());
        mMicroCI->Script() << inja::render(R"(
command -v {{ COMMAND }} &>/dev/null || {
echo "┌──────────────────────────────────────────────────────┐"
echo "│              UNSAFE COMMAND NOT FOUND                │"
echo "├──────────────────────────────────────────────────────┤"
echo "│ {{ COMMAND_CENTERED }} │"
echo "└──────────────────────────────────────────────────────┘"
exit 1; }
)",
                                           data);
      }
    }
  }

  if (step["plugin"]["required"]) {
    if (step["plugin"]["required"]["files"] && step["plugin"]["required"]["files"].IsSequence()) {
      for (auto comm : step["plugin"]["required"]["files"]) {
        data["FILENAME"]          = comm.as<std::string>();
        data["FILENAME_CENTERED"] = fmt::format("{:^52}", comm.as<std::string>());
        mMicroCI->Script() << inja::render(R"(
if [ ! -f {{ FILENAME }} ]; then
  echo "┌──────────────────────────────────────────────────────┐"
  echo "│              UNSAFE FILENAME NOT FOUND               │"
  echo "├──────────────────────────────────────────────────────┤"
  echo "│ {{ FILENAME_CENTERED }} │"
  echo "└──────────────────────────────────────────────────────┘"
exit 1
fi
)",
                                           data);
      }
    }
  }

  // command -v jq &>/dev/null ||
  //   {
  //     echo -e "{{RED}}The utility jq was not found in the system{{CLEAR}}"
  //     echo "{{RED}}Try: {{GREEN}}sudo apt install jq"
  //     echo "{{RED}}Try: {{GREEN}}brew install jq"
  //     exit 1
  //   }

  beginFunction(data, envs);
  // prepareRunDocker(data, envs, volumes);
  //
  // if (step["plugin"]["sh"]) {
  //   mMicroCI->Script() << inja::render(R"( \
  //       /bin/sh)",
  //                                      data);
  // } else if (step["plugin"]["bash"]) {
  //   mMicroCI->Script() << inja::render(R"( \
  //       /bin/bash)",
  //                                      data);
  // } else {
  //   throw std::invalid_argument("No valid shell defined");
  // }
  //
  // for (const auto &opt : opts) {
  //   mMicroCI->Script() << " " << opt;
  // }
  //
  // mMicroCI->Script() << inja::render(R"( -c "cd {{ WORKSPACE }})", data);
  //
  // copySshIfAvailable(step, data);

  mMicroCI->Script() << "           :";
  for (auto cmd : cmds) {
    mMicroCI->Script() << fmt::format(" \\\n           && {} 2>&1", cmd);
  }
  endFunction(data);
  mMicroCI->Script() << "# 🔴🔴🔴 UNSAFE STEP END 🔴🔴🔴\n";
}
}  // namespace microci
