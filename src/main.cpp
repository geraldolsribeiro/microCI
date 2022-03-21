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

#include <argh.h>
#include <spdlog/spdlog.h>

// Plugins
#include <BeamerPluginStepParser.hpp>
#include <ClangFormatPluginStepParser.hpp>
#include <ClangTidyPluginStepParser.hpp>
#include <CppCheckPluginStepParser.hpp>
#include <FetchPluginStepParser.hpp>
#include <GitDeployPluginStepParser.hpp>
#include <GitPublishPluginStepParser.hpp>
#include <MkdocsMaterialPluginStepParser.hpp>
#include <PlantumlPluginStepParser.hpp>
#include <PluginStepParser.hpp>
#include <new/bash.hpp>
#include <new/beamer.hpp>
#include <new/clang-format.hpp>
#include <new/clang-format_config.hpp>
#include <new/clang-tidy.hpp>
#include <new/cppcheck.hpp>
#include <new/fetch.hpp>
#include <new/git_deploy.hpp>
#include <new/git_publish.hpp>
#include <new/mkdocs_material.hpp>
#include <new/mkdocs_material_config.hpp>
#include <new/mkdocs_material_index.hpp>
#include <new/npm.hpp>
#include <new/plantuml.hpp>

// Classe principal
#include <MicroCI.hpp>
using namespace microci;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opções:
  -h --help                Ajuda
  -V --version             Versão
  -O --only                Executa somente o passo especificado
  -i,--input arquivo.yml   Carrega arquivo de configuração
  -n,--new bash            Cria passo bash
  -n,--new mkdocs_material Cria passo para documentação
  -n,--new git_publish     Cria passo para publicar um diretório para repositório
  -n,--new git_deploy      Cria passo para colocar repositório em produção
  -n,--new cppcheck        Cria passo para verificação de código C++
  -n,--new clang-tidy      Cria passo para verificação de código C++
  -n,--new plantuml        Cria passo para geração de diagramas
  -n,--new clang-format    Cria passo para formatação de código
  -n,--new beamer          Cria passo para criação de apresentação PDF
  -n,--new fetch           Cria passo para download de arquivos
)";
}

struct TemplateFile {
  string fileName;
  unsigned char *fileContent;
  unsigned int fileSize;
  bool appendIfExists;
};

using TemplateType = string;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
int main([[maybe_unused]] int argc, char **argv, char **envp) {
  //{{{
  auto yamlfileName = string{".microCI.yml"};
  auto onlyStep = string{};
  auto newType = string{};

  try {
    argh::parser cmdl(argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);

    MicroCI uCI{};
    uCI.RegisterPlugin("bash", make_shared<BashPluginStepParser>(&uCI));
    uCI.RegisterPlugin("beamer", make_shared<BeamerPluginStepParser>(&uCI));
    uCI.RegisterPlugin("plantuml", make_shared<PlantumlPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_deploy",
                       make_shared<GitDeployPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_publish",
                       make_shared<GitPublishPluginStepParser>(&uCI));
    uCI.RegisterPlugin("mkdocs_material",
                       make_shared<MkdocsMaterialPluginStepParser>(&uCI));
    uCI.RegisterPlugin("cppcheck", make_shared<CppCheckPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-tidy",
                       make_shared<ClangTidyPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-format",
                       make_shared<ClangFormatPluginStepParser>(&uCI));
    uCI.RegisterPlugin("fetch", make_shared<FetchPluginStepParser>(&uCI));

    // Carrega as variáveis de ambiente
    for (char **env = envp; *env != 0; env++) {
      auto tmp = string{*env};
      if (tmp.size() > 7 and tmp.substr(0, 8) == "MICROCI_") {
        auto pos = tmp.find_first_of("=");
        EnvironmentVariable environmentVariable;
        environmentVariable.name = tmp.substr(0, pos);
        environmentVariable.value = tmp.substr(pos + 1);
        uCI.SetEnvironmentVariable(environmentVariable);
      }
    }

    if (cmdl[{"-V", "--version"}]) {
      cout << microci::version() << endl;
      return 0;
    }

    if (cmdl[{"-h", "--help"}]) {
      cout << microci::banner() << help() << endl;
      return 0;
    }

    // Aceita um arquivo de configuração num caminho alternativo
    cmdl({"-i", "--input"}) >> yamlfileName;

    // Cria arquivo de configuração
    if ((cmdl({"-n", "--new"}) >> newType)) {
      multimap<TemplateType, TemplateFile> templates;

#define MICROCI_TPL(APPEND_IF_EXISTS, TYPE, FILE_NAME, FILE_EXTENSION,      \
                    INCLUDE_VAR_NAME)                                       \
  templates.insert(make_pair(                                               \
      TYPE,                                                                 \
      TemplateFile{FILE_NAME, ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION, \
                   ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION##_len,      \
                   APPEND_IF_EXISTS}));

      MICROCI_TPL(true, "bash", ".microCI.yml", yml, bash);
      MICROCI_TPL(true, "clang-tidy", ".microCI.yml", yml, clang_tidy);
      MICROCI_TPL(true, "cppcheck", ".microCI.yml", yml, cppcheck);
      MICROCI_TPL(true, "git_deploy", ".microCI.yml", yml, git_deploy);
      MICROCI_TPL(true, "git_publish", ".microCI.yml", yml, git_publish);
      MICROCI_TPL(false, "mkdocs_material", "docs/index.md", md,
                  mkdocs_material_index);
      MICROCI_TPL(true, "mkdocs_material", ".microCI.yml", yml,
                  mkdocs_material);
      MICROCI_TPL(false, "mkdocs_material", "mkdocs.yml", yml,
                  mkdocs_material_config);
      MICROCI_TPL(true, "npm", ".microCI.yml", yml, npm);
      MICROCI_TPL(true, "plantuml", ".microCI.yml", yml, plantuml);
      MICROCI_TPL(true, "clang-format", ".microCI.yml", yml, clang_format);
      MICROCI_TPL(false, "clang-format", ".clang-format", yml,
                  clang_format_config);
      MICROCI_TPL(true, "beamer", ".microCI.yml", yml, beamer);
      MICROCI_TPL(true, "fetch", ".microCI.yml", yml, fetch);
#undef MICROCI_TPL

      bool isTypeFound = false;
      for (const auto &[type, tpl] : templates) {
        if (newType == type) {
          isTypeFound = true;

          ofstream out;
          auto fileName = tpl.fileName;

          // Caso seja utilizado algum path diferente do default
          if (fileName == ".microCI.yml") {
            fileName = yamlfileName;
          }

          auto folderPos = fileName.find_last_of("/");
          if (folderPos != string::npos) {
            auto folderName = fileName.substr(0, folderPos);
            spdlog::info("Criando diretório {}", folderName);
            filesystem::create_directories(folderName);
          }

          if (!tpl.appendIfExists and filesystem::exists(fileName)) {
            spdlog::info("Ignorando criação, pois o arquivo {} já existe",
                         fileName);
            continue;
          } else if (tpl.appendIfExists and filesystem::exists(fileName)) {
            out.open(fileName, ios_base::app);
            out << "\n# ---------- "
                   "MESCLE MANUALMENTE CONTEÚDO ABAIXO "
                   "---------\n";
            spdlog::info("Arquivo {} foi editado a partir do modelo", fileName);
          } else {
            out.open(fileName);
            spdlog::info("Arquivo {} foi criado a partir do modelo", fileName);
          }

          out.write((char *)tpl.fileContent, tpl.fileSize);
        }
      }
      if (isTypeFound) {
        return 0;
      }
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

    cout << uCI.ToString();
  } catch (invalid_argument &e) {
    spdlog::error(e.what());
    return 1;
  } catch (...) {
    spdlog::error("Exceção desconhecida");
    return 1;
  }
  return 0;
}
