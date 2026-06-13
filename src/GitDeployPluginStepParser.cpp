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

#include "GitDeployPluginStepParser.hpp"

#include <fstream>
#include <spdlog/spdlog.h>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void GitDeployPluginStepParser::Parse(const YAML::Node &step) {
  auto envs = parseEnvs(step);
  auto data = mMicroCI->DefaultDataTemplate();
  data      = parseRunAs(step, data, "user");

  const auto name     = step["plugin"]["name"].as<std::string>();
  const auto repo     = step["plugin"]["repo"].as<std::string>();
  const auto gitDir   = step["plugin"]["git_dir"].as<std::string>();
  const auto workTree = step["plugin"]["work_tree"].as<std::string>();
  auto clean          = true;

  if (step["plugin"]["clean"]) {
    clean = step["plugin"]["clean"].as<bool>();
  }

  data["GIT_URL"]          = repo;
  data["GIT_DIR"]          = gitDir;
  data["GIT_WORK"]         = workTree;
  data["STEP_NAME"]        = stepName(step);
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step);

  beginFunction(data, envs);
  mMicroCI->Script() << inja::render(R"(
      # If it does not exist yet, perform the initial clone
      if [ ! -d "{{GIT_DIR}}" ]; then
        git clone "{{GIT_URL}}" \
          --separate-git-dir="{{GIT_DIR}}" \
          "{{GIT_WORK}}" 2>&1
      fi

)",
                                     data);

  if (clean) {
    mMicroCI->Script() << inja::render(R"(
      # Clean the folder -- USE WITH CARE WHEN MERGING REPOS
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        clean -xfd 2>&1
)",
                                       data);
  }

  mMicroCI->Script() << inja::render(R"(
      # Check out the current version
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        checkout -f 2>&1 \
      && git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        pull 2>&1

      # Remove the .git file that points to the git-dir
      rm -f "{{GIT_WORK}}/.git" 2>&1

      date
)",
                                     data);
  endFunction(data);
}
}  // namespace microci
