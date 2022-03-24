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

#include <MinioPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void MinioPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);

  // list<string> sourceList;
  //
  // data = parseRunAs(step, data, "user");
  // data = parseNetwork(step, data);
  //
  // if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
  //   for (const auto &src : step["plugin"]["source"]) {
  //     sourceList.push_back(src.as<string>());
  //   }
  // }

  for (const auto &envName : {"MICROCI_MINIO_URL", "MICROCI_MINIO_ACCESS_KEY", "MICROCI_MINIO_SECRET_KEY"}) {
    auto it = envs.find(EnvironmentVariable{envName, ""});
    if (it == envs.end()) {
      spdlog::error("Variável de ambiente '{}' não definida", envName);
      mIsValid = false;
      return;
    } else {
      data[envName] = it->value;
    }
  }

  data["STEP_NAME"] = stepName(step);
  data["DOCKER_IMAGE"] = stepDockerImage(step, "minio/mc");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Envia arquivos para gerenciador de artefatos");
  data["DOCKER_ENTRYPOINT"] = "";  // remove o entrypoint padrão
  data["DOCKER_NETWORK"] = "bridge";

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);
  mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }} \
        && mc alias set minio {{MICROCI_MINIO_URL}} {{MICROCI_MINIO_ACCESS_KEY}} {{MICROCI_MINIO_SECRET_KEY}} --api S3v4 \
        && mc ls minio/rafael)",
                                     data);
  mMicroCI->Script() << R"("
)";
  endFunction(data);
}
}  // namespace microci
