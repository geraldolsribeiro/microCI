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

#include <GitDeployPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void GitDeployPluginStepParser::Parse(const YAML::Node& step) {
  const auto name = step["plugin"]["name"].as<string>();
  const auto repo = step["plugin"]["repo"].as<string>();
  const auto gitDir = step["plugin"]["git_dir"].as<string>();
  const auto workTree = step["plugin"]["work_tree"].as<string>();
  auto clean = true;

  if (step["plugin"]["clean"]) {
    clean = step["plugin"]["clean"].as<bool>();
  }

  auto data = mMicroCI->DefaultDataTemplate();
  data["RUN_AS"] = "root";
  data["GIT_URL"] = repo;
  data["GIT_DIR"] = gitDir;
  data["GIT_WORK"] = workTree;
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step);

  auto envs = parseEnvs(step);

  beginFunction(data, envs);
  mMicroCI->Script() << inja::render(R"(
      # Caso ainda não exista realiza o clone inicial
      if [ ! -d "{{GIT_DIR}}" ]; then
        git clone "{{GIT_URL}}" \
          --separate-git-dir="{{GIT_DIR}}" \
          "{{GIT_WORK}}" 2>&1
      fi

)",
                                     data);

  if (clean) {
    mMicroCI->Script() << inja::render(R"(
      # Limpa a pasta -- CUIDADO AO MESCLAR REPOS
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        clean -xfd 2>&1
)",
                                       data);
  }

  mMicroCI->Script() << inja::render(R"(
      # Extrai a versão atual
      git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        checkout -f 2>&1 \
      && git --git-dir="{{GIT_DIR}}" \
        --work-tree="{{GIT_WORK}}" \
        pull 2>&1

      # Remove o arquivo .git que aponta para o git-dir
      rm -f "{{GIT_WORK}}/.git" 2>&1

      date
)",
                                     data);
  endFunction(data);
}  // namespace microci
}  // namespace microci
