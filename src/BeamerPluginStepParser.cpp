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

#include <spdlog/spdlog.h>

#include <BeamerPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void BeamerPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");

  data["STEP_NAME"] = stepName(step);
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Apresentação PDF criada a partir do markdown");
  data["DOCKER_IMAGE"] = stepDockerImage(step, "pandoc/latex:latest");
  data["WORKSPACE"] = stepDockerWorkspace(step, "/data");

  auto inputMD = string{};
  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &opt : step["plugin"]["source"]) {
      inputMD += opt.as<string>() + " ";
    }
  }
  if (!inputMD.empty()) {
    data["INPUT_MD"] = inputMD;
  } else {
    spdlog::error("É obrigatório especificar uma lista de arquivos de entrada");
    throw std::runtime_error("É obrigatório especificar uma lista de arquivos de entrada");
  }

  data["LANG"] = step["plugin"]["lang"].as<string>("pt-BR");
  data["DATE"] = step["plugin"]["date"].as<string>("01 de Abril de 2023");
  data["INSTITUTE"] = step["plugin"]["institute"].as<string>("Nome da instituição");
  data["TITLE"] = step["plugin"]["title"].as<string>("Título da apresentação");
  data["SUBTITLE"] = step["plugin"]["subtitle"].as<string>("Subtítulo da apresentação");
  data["SUBJECT"] = step["plugin"]["subject"].as<string>("Informação da propriedade Assunto do PDF");
  data["SLIDE_LEVEL"] = step["plugin"]["slide-level"].as<string>("2");
  data["ASPECTRATIO"] = step["plugin"]["aspectratio"].as<string>("169");
  data["OUTPUT_PDF"] = step["plugin"]["output"].as<string>("output.pdf");

  // data["HEADER_INCLUDES"] = "";
  auto filename = string{"header-includes.yaml"};
  ofstream yaml(filename);
  YAML::Node strippedHeaderIncludes = Clone(step["plugin"]);

  if (step["plugin"]["theme"].as<string>("default") == "STR") {
    strippedHeaderIncludes["header-includes"].push_back(R"(\useoutertheme{shadow})");
    strippedHeaderIncludes["header-includes"].push_back(R"(\usecolortheme{str})");
    strippedHeaderIncludes["header-includes"].push_back(R"(\logo{\includegraphics[height=7mm]{img/str-logo.png}})");

// Encontrar uma solução mais elegante para disponibilizar arquivos de terceiros
#include <3rd/beamercolorthemestr.sty.hpp>
#include <3rd/str-logo.png.hpp>
    ofstream strLogoPng("img/str-logo.png");
    strLogoPng.write((char *)___3rd_str_logo_png, ___3rd_str_logo_png_len);
    ofstream beamercolorthemestr("beamercolorthemestr.sty");
    beamercolorthemestr.write((char *)___3rd_beamercolorthemestr_sty, ___3rd_beamercolorthemestr_sty_len);
  }

  for (const auto &key : {/* keys a seguir não são usadas pelo pandoc: */ "name", "output", "source", "theme",
                          /* keys a seguir já foram passadas via linha de comando: */ "lang", "date", "institute",
                          "title", "subtitle", "subject", "slide-level", "aspectratio"}) {
    strippedHeaderIncludes.remove(key);
  }

  yaml << "---\n";
  yaml << "# Atenção, este arquivo é gerado automaticamente!\n";
  yaml << "# Se necessário edite o arquivo .microCI.yml\n";
  yaml << strippedHeaderIncludes;
  yaml << "\n..." << endl;
  data["HEADER_INCLUDES"] = filename;

  if (step["plugin"]["natbib"] and step["plugin"]["natbib"].as<bool>()) {
    data["CITEPROC"] = "--citeproc";
  } else {
    data["CITEPROC"] = "";
  }

  auto envs = parseEnvs(step);
  beginFunction(data, envs);

  mMicroCI->Script() << inja::render(R"(
      # shellcheck disable=SC2140
      docker run \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_{{ FUNCTION_NAME }} \
        --workdir {{ WORKSPACE }} \
        --volume "${MICROCI_PWD}":{{ WORKSPACE }} \
        --user $(id -u):$(id -g) \
        {{ DOCKER_IMAGE }} \
        --variable lang='{{ LANG }}' \
        --variable date='{{ DATE }}' \
        --variable institute='{{ INSTITUTE }}' \
        --variable title='{{ TITLE }}' \
        --variable subtitle='{{ SUBTITLE }}' \
        --variable subject='{{ SUBJECT }}' \
        --variable aspectratio={{ ASPECTRATIO }} \
        --slide-level={{ SLIDE_LEVEL }} \
        --to=beamer \
        {{ CITEPROC }} \
        {{ HEADER_INCLUDES }} \
        {{ INPUT_MD }} \
        -o {{ OUTPUT_PDF }} \
        2>&1; \
      rm -f {{ HEADER_INCLUDES }} img/str-logo.png beamercolorthemestr.sty 2>&1
)",
                                     data);

  endFunction(data);
}
}  // namespace microci
