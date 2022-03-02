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

#include <new/bash.hpp>
#include <new/clang-tidy.hpp>
#include <new/cppcheck.hpp>
#include <new/git_deploy.hpp>
#include <new/git_publish.hpp>
#include <new/mkdocs_material.hpp>
#include <new/mkdocs_material_config.hpp>
#include <new/mkdocs_material_index.hpp>
#include <new/npm.hpp>
#include <new/plantuml.hpp>

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opções:
  -h --help                Ajuda
  -i,--input arquivo.yml   Carrega arquivo de configuração
  -n,--new tipo            [bash|mkdocs_material|git_deploy|git_publish
                            cppcheck|clang-tidy|plantuml]

)";
}

struct TemplateFile {
  string fileName;
  unsigned char *fileContent;
  unsigned int fileSize;
};

using TemplateType = string;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  //{{{
  argh::parser cmdl(argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);
  auto yamlfileName = string{".microCI.yml"};
  auto onlyStep = string{};
  auto newType = string{};

  MicroCI uCI{};

  if (cmdl[{"-h", "--help"}]) {
    cout << microci::banner() << help() << endl;
    return 0;
  }

  // Aceita um arquivo de configuração num caminho alternativo
  cmdl({"-i", "--input"}) >> yamlfileName;

  // Cria arquivo de configuração
  if ((cmdl({"-n", "--new"}) >> newType)) {
    multimap<TemplateType, TemplateFile> templates;

#define MICROCI_TPL(TYPE, FILE_NAME, FILE_EXTENSION, INCLUDE_VAR_NAME)      \
  templates.insert(make_pair(                                               \
      TYPE,                                                                 \
      TemplateFile{FILE_NAME, ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION, \
                   ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION##_len}));

    MICROCI_TPL("bash", ".microCI.yml", yml, bash);
    MICROCI_TPL("clang-tidy", ".microCI.yml", yml, clang_tidy);
    MICROCI_TPL("cppcheck", ".microCI.yml", yml, cppcheck);
    MICROCI_TPL("git_deploy", ".microCI.yml", yml, git_deploy);
    MICROCI_TPL("git_publish", ".microCI.yml", yml, git_publish);
    MICROCI_TPL("mkdocs_material", "docs/index.md", md, mkdocs_material_index);
    MICROCI_TPL("mkdocs_material", ".microCI.yml", yml, mkdocs_material);
    MICROCI_TPL("mkdocs_material", "mkdocs.yml", yml, mkdocs_material_config);
    MICROCI_TPL("npm", ".microCI.yml", yml, npm);
    MICROCI_TPL("plantuml", ".microCI.yml", yml, plantuml);

    bool found = false;

    for (const auto &[type, tpl] : templates) {
      if (newType == type) {
        found = true;

        ofstream out;
        auto fileName = tpl.fileName;

        // Caso seja utilizado algum path diferente do defaul
        if (fileName == ".microCI.yml") {
          fileName = yamlfileName;
        }

        auto folderPos = fileName.find_last_of("/");
        if (folderPos != string::npos) {
          auto folderName = fileName.substr(0, folderPos);
          spdlog::info("Criando diretório {}", folderName);
          filesystem::create_directories(folderName);
        }

        if (filesystem::exists(fileName)) {
          out.open(fileName, ios_base::app);
          out << "\n# ---------- "
                 "MESCLE MANUALMENTE CONTEÚDO ABAIXO "
                 "---------\n";
        } else {
          out.open(fileName);
        }

        out.write((char *)tpl.fileContent, tpl.fileSize);
        spdlog::info("Arquivo {} foi editado a partir do modelo", fileName);
      }
    }
    if (found) {
      return 0;
    }
#undef MICROCI_TPL
    spdlog::error("Impossível criar para tipo inválido: {}", newType);
    for (auto it = templates.begin(), end = templates.end(); it != end;
         it = templates.upper_bound(it->first)) {
      spdlog::info("Exemplo: microCI --new {}", it->first);
    }
    return -1;
  }

  if (!filesystem::exists(yamlfileName)) {
    cout << microci::banner() << endl;
    spdlog::error("Arquivo de entrada {} não encontrado.", yamlfileName);
    return 1;
  }

  if ((cmdl({"-O", "--only"}) >> onlyStep)) {
    uCI.SetOnlyStep(onlyStep);
  }

  if (!uCI.ReadConfig(yamlfileName)) {
    cout << microci::banner() << endl;
    spdlog::error("Falha na leitura do arquivo {}", yamlfileName);
    return 1;
  }

  cout << uCI.Script();
  return 0;
}

