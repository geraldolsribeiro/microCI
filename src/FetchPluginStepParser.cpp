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

#include <FetchPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void FetchPluginStepParser::Parse(const YAML::Node& step) {
  auto data = mMicroCI->DefaultDataTemplate();
  data["DOCKER_NETWORK"] = "bridge";
  data = parseRunAs(step, data);
  data = parseNetwork(step, data);
  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Baixa arquivos externos ao projeto");
  data["DOCKER_IMAGE"] = stepDockerImage(step, "bitnami/git:latest");

  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  if (step["plugin"]["items"] && step["plugin"]["items"].IsSequence()) {
    auto defaultTarget = step["plugin"]["target"].as<string>("include/");

    beginFunction(data, envs);
    prepareRunDocker(data, envs, volumes);
    mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                       data);
    copySshIfAvailable(step, data);

    for (const auto& item : step["plugin"]["items"]) {
      if (item["git_archive"]) {
        auto files = string{};
        for (const auto& f : item["files"]) {
          files += fmt::format(
              "'{}' ", f.as<string>());  // aspas simples para não expandir
        }
        if (files.empty()) {
          throw std::runtime_error(
              "É obrigatório especificar uma lista de arquivos de entrada");
        }
        data["FILES"] = files;
        data["GIT_REMOTE"] = item["git_archive"].as<string>();
        data["TARGET"] = item["target"].as<string>(defaultTarget);

        mMicroCI->Script() << inja::render(
            R"( \
           && mkdir -p {{ TARGET }} \
           && git archive --format=tar --remote={{ GIT_REMOTE }} HEAD {{ FILES }} \
             | tar -C {{ TARGET }} -vxf - 2>&1)",
            data);
      } else if (item["url"]) {
        data["TARGET"] = item["target"].as<string>(defaultTarget);
        data["URL"] = item["url"].as<string>();
        mMicroCI->Script() << inja::render(
            R"( \
           && mkdir -p {{ TARGET }} \
           && pushd {{ TARGET }} \
           && curl -fSL -R -J -O {{ URL }} 2>&1 \
           && popd)",
            data);
      }
    }
    mMicroCI->Script() << "\"\n";
    endFunction(data);
  }
}  // namespace microci
}  // namespace microci
