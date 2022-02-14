// ░░░░░░░░░░░░░░░░░ microCI
// ░░░░░░░█▀▀░▀█▀░░░
// ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
// ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// ░░░▀░░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░ Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

#include <spdlog/spdlog.h>

#include <MicroCI.hpp>
#include <argh.hpp>
using namespace microci;

#include <new/git_deploy.hpp>
#include <new/mkdocs_material.hpp>
#include <new/npm.hpp>

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opções:
  -h --help                Ajuda
  -i,--input arquivo.yml   Carrega arquivo de configuração (default .microCI.yml)
  -n,--new tipo            [bash|mkdocs_material|git_deploy]

)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  //{{{
  argh::parser cmdl(argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);
  auto yamlFilename = string{".microCI.yml"};
  auto onlyStep = string{};
  auto newType = string{};

  MicroCI uCI{};

  if (cmdl[{"-h", "--help"}]) {
    cout << microci::banner() << help() << endl;
    return 0;
  }

  cmdl({"-i", "--input"}) >> yamlFilename;

  if ((cmdl({"-c", "--create"}) >> newType)) {
    map<string, pair<unsigned char *, unsigned int>> newTypeTemplates;
#define YAML_TPL(X) \
  newTypeTemplates[#X] = make_pair(___new_##X##_yml, ___new_##X##_yml_len)
    YAML_TPL(mkdocs_material);
    YAML_TPL(npm);
    YAML_TPL(git_deploy);
#undef YAML_TPL

    if (newTypeTemplates.count(newType)) {
      auto [buf, len] = newTypeTemplates[newType];
      ofstream out(yamlFilename);
      out.write((char *)buf, len);
      cout.write((char *)buf, len);
      return 0;
    }
    spdlog::error("Impossível criar para tipo inválido: {}", newType);
    for (const auto tpl : newTypeTemplates) {
      spdlog::info("Exemplo: microCI --create {}", tpl.first);
    }
    return -1;
  }

  if (!filesystem::exists(yamlFilename)) {
    cout << microci::banner() << endl;
    spdlog::error("Arquivo de entrada {} não encontrado.", yamlFilename);
    return 1;
  }

  if ((cmdl({"-O", "--only"}) >> onlyStep)) {
    uCI.SetOnlyStep(onlyStep);
  }

  if (!uCI.ReadConfig(yamlFilename)) {
    cout << microci::banner() << endl;
    spdlog::error("Falha na leitura do arquivo {}", yamlFilename);
    return 1;
  }

  cout << uCI.Script();
  return 0;
}

