// TODO: saída para relatório
// https://github.com/p-ranav/tabulate
// https://docs.docker.com/engine/reference/commandline/run/
// https://plantuml.com/yaml

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

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opções:
  -h --help                Ajuda
  -i,--input arquivo.yml   Carrega arquivo de configuração (default .microCI.yml)

)";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  //{{{
  argh::parser cmdl(argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);
  string yamlFilename = ".microCI.yml";
  MicroCI uCI{};

  if (cmdl[{"-h", "--help"}]) {
    cout << microci::banner() << help() << endl;
    return 0;
  }

  cmdl({"-i", "--input"}) >> yamlFilename;

  if (!filesystem::exists(yamlFilename)) {
    cout << microci::banner() << endl;
    spdlog::error("Arquivo de entrada {} não encontrado.", yamlFilename);
    return 1;
  }

  if (!uCI.ReadConfig(yamlFilename)) {
    cout << microci::banner() << endl;
    spdlog::error( "Falha na leitura do arquivo {}", yamlFilename );
    return 1;
  }

  cout << uCI.Script();
  return 0;
}

