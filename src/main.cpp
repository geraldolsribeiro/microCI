// TODO: saída para relatório
// https://github.com/p-ranav/tabulate
// https://docs.docker.com/engine/reference/commandline/run/
// https://plantuml.com/yaml
//
//
// FIXME: 5 Simple Steps On How To Debug A Bash Shell Script
// https://www.shell-tips.com/bash/debug-script/

// 📎 You can replace the default + character used in the xtrace output by
// changing the Bash Prompt $PS4 Variableicon mdi-link-variant.

/*
 *https://codefresh.io/docs/docs/learn-by-example/cc/c-make/
version: '1.0'
stages:
  - checkout
  - build
steps:
  main_clone:
    title: Cloning main repository...
    stage: checkout
    type: git-clone
    repo: 'codefresh-contrib/c-sample-app'
    revision: master
    git: github
  compile_my_sources:
    title: Compile
    stage: build
    image: gcc
    commands:
      - make
  run_my_tests:
    title: Test
    stage: build
    image: gcc
    commands:
      - make test
      */

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

