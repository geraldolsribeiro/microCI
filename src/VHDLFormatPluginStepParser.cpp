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

#include <VHDLFormatPluginStepParser.hpp>
#include <fstream>

namespace microci {
using namespace std;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void VHDLFormatPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  auto volumes = parseVolumes(step);
  auto envs = parseEnvs(step);
  data = parseRunAs(step, data, "user");
  data = parseNetwork(step, data, "none");
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);
  list<string> sourceList;

  if (step["plugin"]["source"] && step["plugin"]["source"].IsSequence()) {
    for (const auto &src : step["plugin"]["source"]) {
      sourceList.push_back(src.as<string>());
    }
  }

  auto upperCaseOption{false};
  if (step["plugin"]["uppercase"]) {
    upperCaseOption = step["plugin"]["uppercase"].as<bool>(false);
  }

  data["STEP_NAME"] = stepName(step);
  data["STEP_DESCRIPTION"] =
      stepDescription(step, "Process source code to make readable or match to a project code style");
  data["FUNCTION_NAME"] = sanitizeName(stepName(step));
  data["DOCKER_IMAGE"] = stepDockerImage(step, "intmain/microci_ghdl:latest");

  auto emacsConfigFilename = inja::render("{{ WORKSPACE }}/.emacs_vhdl_formatter.lisp", data);
  data["EMACS_VHDL_FORMATTER_FILENAME"] = emacsConfigFilename;

  ostringstream emacsConfig;
  emacsConfig << inja::render(R"(;; microCI emacs configuration used for formatting VHDL code
(custom-set-variables
;; '(vhdl-align-group-separate "^\\s---*$")
  '(vhdl-align-groups t)
  '(vhdl-align-same-indent t)
  '(vhdl-auto-align t)
  '(vhdl-basic-offset 2)
  '(vhdl-beautify-options '(t      ;; whitespace cleanup
                            t      ;; single statement per line
                            t      ;; indentation
                            t      ;; aligment
                            t))    ;; case fixing
  ;; '(vhdl-standard '(8 nil))
  '(vhdl-standard '(8 (ams math))) ;; VHDL-08
  '(vhdl-array-index-record-field-in-sensitivity-list t)
  '(vhdl-use-direct-instantiation 'always)
)",
                              data);

  if (upperCaseOption) {
    emacsConfig << inja::render(R"(
  '(vhdl-upper-case-attributes t)
  '(vhdl-upper-case-constants t)
  '(vhdl-upper-case-enum-values t)
  '(vhdl-upper-case-keywords t)
  '(vhdl-upper-case-types t)
 )",
                                data);
  }

  emacsConfig << R"(
))";

  data["EMACS_VHDL_FORMATTER_CONFIG"] = emacsConfig.str();

  beginFunction(data, envs);
  prepareRunDocker(data, envs, volumes);

  mMicroCI->Script() << inja::render(R"( \
        bash -c "cd {{ WORKSPACE }}
        cat <<EOF > {{ EMACS_VHDL_FORMATTER_FILENAME }}
{{ EMACS_VHDL_FORMATTER_CONFIG }}
EOF
        echo "Formatting...")",
                                     data);

  for (const auto &src : sourceList) {
    mMicroCI->Script() << fmt::format(R"( \
        && cat <(compgen -G '{}') \
          | )",
                                      src);
    mMicroCI->Script() << inja::render(R"(xargs --no-run-if-empty -I {} emacs -batch \
            -l {{ EMACS_VHDL_FORMATTER_FILENAME }} \
            {} -f vhdl-beautify-buffer 2>&1 )",
                                       data);
  }
  mMicroCI->Script() << inja::render(R"( \
        && rm -f {{ EMACS_VHDL_FORMATTER_FILENAME }})",
                                     data);

  mMicroCI->Script() << "\"\n";
  endFunction(data);
}
}  // namespace microci
