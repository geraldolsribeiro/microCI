// MIT License
//
// Copyright (C) 2022-2024 Geraldo Luis da Silva Ribeiro
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

// https://stackoverflow.com/questions/69806220/advice-needed-for-migration-of-low-level-openssl-api-to-high-level-openssl-apis

#include <openssl/evp.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

#include <argh.h>
#include <inicpp.h>
#include <libintl.h>
#include <spdlog/spdlog.h>

#define _(String) gettext(String)

// Plugins
#include "BashPluginStepParser.hpp"
#include "BeamerPluginStepParser.hpp"
#include "ClangFormatPluginStepParser.hpp"
#include "ClangTidyPluginStepParser.hpp"
#include "CppCheckPluginStepParser.hpp"
#include "DocmdPluginStepParser.hpp"
#include "FetchPluginStepParser.hpp"
#include "FlawfinderPluginStepParser.hpp"
#include "GitDeployPluginStepParser.hpp"
#include "GitPublishPluginStepParser.hpp"
#include "MinioPluginStepParser.hpp"
#include "MkdocsMaterialPluginStepParser.hpp"
#include "PandocPluginStepParser.hpp"
#include "PikchrPluginStepParser.hpp"
#include "PlantumlPluginStepParser.hpp"
#include "PluginStepParser.hpp"
#include "TemplatePluginStepParser.hpp"

// Configuration templates
#include "new/bash.hpp"
#include "new/beamer.hpp"
#include "new/clang-format.hpp"
#include "new/clang-format_config.hpp"
#include "new/clang-tidy.hpp"
#include "new/cppcheck.hpp"
#include "new/docker_build.hpp"
#include "new/docmd.hpp"
#include "new/fetch.hpp"
#include "new/flawfinder.hpp"
#include "new/git_deploy.hpp"
#include "new/git_publish.hpp"
#include "new/gitlab-ci.hpp"
#include "new/minio.hpp"
#include "new/mkdocs_material.hpp"
#include "new/mkdocs_material_config.hpp"
#include "new/mkdocs_material_index.hpp"
#include "new/npm.hpp"
#include "new/pandoc.hpp"
#include "new/pikchr.hpp"
#include "new/plantuml.hpp"
#include "new/skip.hpp"
#include "new/template.hpp"

// main class
#include "MicroCI.hpp"

using namespace microci;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
string help() {
  return R"(
Opions:
  -h,--help                Print this help
  -V,--version             Print the microCI version
  -T,--test-config         Configuration test
  -A,--activity-diagram    Generate activity diagram
  -a,--append-log          Append log
  -O,--only                Execute only a single step
  -U,--update-db           Update observability database
  -u,--update              Update microCI
  -i,--input file.yml      Load the configuration from file.yml
  -n,--config gitlab-ci    Create a .gitlab-ci.yml example config
  -n,--new skip            Create a placeholder step
  -n,--new bash            Create a command line step
  -n,--new docmd           Create a documentation step
  -n,--new mkdocs_material Create a documentation step
  -n,--new pandoc          Create a document conversion step
  -n,--new git_publish     Create a publish step
  -n,--new git_deploy      Create a production deploy step
  -n,--new plantuml        Create a diagram generation step
  -n,--new pikchr          Create a diagram generation step
  -n,--new clang-format    Create a code format step
  -n,--new beamer          Create a PDF presentation step
  -n,--new fetch           Create a download external artfact step
  -n,--new minio           Create a upload/download internal artifact step
  -n,--new cppcheck        Create a C++ SAST step
  -n,--new clang-tidy      Create a C++ SAST step
  -n,--new flawfinder      Create a C++ SAST step
  -n,--new docker_build    Create a local docker build step
  -n,--new template        Create a template step
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

  try {
    setlocale(LC_ALL, "");
    bindtextdomain("microci", NULL);
    bind_textdomain_codeset("microci", "UTF-8");
    textdomain("microci");

    spdlog::set_pattern("# MicroCI [%H:%M:%S %z] [%^%l%$] %v");

    argh::parser cmdl(argv, argh::parser::Mode::PREFER_PARAM_FOR_UNREG_OPTION);

    if (cmdl[{"-u", "--update"}]) {
      cout << "sudo curl -fsSL "
              "github.com/geraldolsribeiro/microci/releases/latest/download/microCI "
              "-o /usr/bin/microCI\n"
              "sudo chmod 755 /usr/bin/microCI"
           << endl;
      return 0;
    }

    MicroCI uCI{};

    uCI.SetAppendLog(cmdl[{"-a", "--append-log"}]);

    uCI.RegisterPlugin("skip", make_shared<SkipPluginStepParser>(&uCI));
    uCI.RegisterPlugin("bash", make_shared<BashPluginStepParser>(&uCI));
    uCI.RegisterPlugin("docmd", make_shared<DocmdPluginStepParser>(&uCI));
    uCI.RegisterPlugin("beamer", make_shared<BeamerPluginStepParser>(&uCI));
    uCI.RegisterPlugin("plantuml", make_shared<PlantumlPluginStepParser>(&uCI));
    uCI.RegisterPlugin("pikchr", make_shared<PikchrPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_deploy", make_shared<GitDeployPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_publish", make_shared<GitPublishPluginStepParser>(&uCI));
    uCI.RegisterPlugin("mkdocs_material", make_shared<MkdocsMaterialPluginStepParser>(&uCI));
    uCI.RegisterPlugin("cppcheck", make_shared<CppCheckPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-tidy", make_shared<ClangTidyPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-format", make_shared<ClangFormatPluginStepParser>(&uCI));
    uCI.RegisterPlugin("fetch", make_shared<FetchPluginStepParser>(&uCI));
    uCI.RegisterPlugin("minio", make_shared<MinioPluginStepParser>(&uCI));
    uCI.RegisterPlugin("flawfinder", make_shared<FlawfinderPluginStepParser>(&uCI));
    uCI.RegisterPlugin("docker_build", make_shared<DockerBuildPluginStepParser>(&uCI));
    uCI.RegisterPlugin("pandoc", make_shared<PandocPluginStepParser>(&uCI));
    uCI.RegisterPlugin("template", make_shared<TemplatePluginStepParser>(&uCI));

    // Load environment variables
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

    // Print version and exist
    if (cmdl[{"-V", "--version"}]) {
      cout << microci::version() << endl;
      return 0;
    }

    // Print help and exist
    if (cmdl[{"-h", "--help"}]) {
      cout << microci::banner() << help() << endl;
      return 0;
    }

    // Alternative path for the configuration file
    auto yamlFileName = string{".microCI.yml"};
    cmdl({"-i", "--input"}) >> yamlFileName;

    auto newConfig = string{};
    if ((cmdl({"-c", "--config"}) >> newConfig)) {
      multimap<TemplateType, TemplateFile> templates;

#define MICROCI_TPL(APPEND_IF_EXISTS, TYPE, FILE_NAME, FILE_EXTENSION, INCLUDE_VAR_NAME)    \
  templates.insert(                                                                         \
      make_pair(TYPE, TemplateFile{FILE_NAME, ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION, \
                                   ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION##_len, APPEND_IF_EXISTS}));
      // clang-format off
      MICROCI_TPL(false, "gitlab_ci",       ".gitlab-ci.yml", yml, gitlab_ci);
      // clang-format on

      bool isNewConfigFound = false;
      for (const auto &[config, tpl] : templates) {
        if (newConfig == config) {
          isNewConfigFound = true;

          ofstream out;
          auto fileName = tpl.fileName;

          auto folderPos = fileName.find_last_of("/");
          if (folderPos != string::npos) {
            auto folderName = fileName.substr(0, folderPos);
            spdlog::debug(_("Creating folder '{}'"), folderName);
            filesystem::create_directories(folderName);
          }

          if (!tpl.appendIfExists and filesystem::exists(fileName)) {
            spdlog::debug(_("File '{}' already exists"), fileName);
            continue;
          } else if (tpl.appendIfExists and filesystem::exists(fileName)) {
            spdlog::debug(_("The file '{}' was edited from the template"), fileName);
            string step{(char *)tpl.fileContent, tpl.fileSize};
            step.erase(0, step.find("steps:") + 7);
            out.open(fileName, ios_base::app);
            out << "\n# --- PLEASE MERGE THE CONTENT BELOW TO YOUR CONFIG ---\n";
            out << step;
          } else {
            spdlog::debug(_("The config file '{}' was created from the template"), fileName);
            out.open(fileName);
            out.write((char *)tpl.fileContent, tpl.fileSize);
          }
        }
      }
      if (isNewConfigFound) {
        return 0;
      }
      spdlog::error(_("Invalid config type: {}"), newConfig);
      for (auto it = templates.begin(), end = templates.end(); it != end; it = templates.upper_bound(it->first)) {
        spdlog::debug(_("Use: microCI --config {}"), it->first);
      }
      return -1;
#undef MICROCI_TPL
    }

    // Create or update the configuration file
    auto newType = string{};
    if ((cmdl({"-n", "--new"}) >> newType)) {
      multimap<TemplateType, TemplateFile> templates;

#define MICROCI_TPL(APPEND_IF_EXISTS, TYPE, FILE_NAME, FILE_EXTENSION, INCLUDE_VAR_NAME)    \
  templates.insert(                                                                         \
      make_pair(TYPE, TemplateFile{FILE_NAME, ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION, \
                                   ___new_##INCLUDE_VAR_NAME##_##FILE_EXTENSION##_len, APPEND_IF_EXISTS}));
      // clang-format off
      MICROCI_TPL(true,  "skip",            ".microCI.yml",  yml, skip);
      MICROCI_TPL(true,  "bash",            ".microCI.yml",  yml, bash);
      MICROCI_TPL(true,  "docmd",           ".microCI.yml",  yml, docmd);
      MICROCI_TPL(true,  "clang-tidy",      ".microCI.yml",  yml, clang_tidy);
      MICROCI_TPL(true,  "cppcheck",        ".microCI.yml",  yml, cppcheck);
      MICROCI_TPL(true,  "git_deploy",      ".microCI.yml",  yml, git_deploy);
      MICROCI_TPL(true,  "git_publish",     ".microCI.yml",  yml, git_publish);
      MICROCI_TPL(false, "mkdocs_material", "docs/index.md", md,  mkdocs_material_index);
      MICROCI_TPL(true,  "mkdocs_material", ".microCI.yml",  yml, mkdocs_material);
      MICROCI_TPL(false, "mkdocs_material", "mkdocs.yml",    yml, mkdocs_material_config);
      MICROCI_TPL(true,  "npm",             ".microCI.yml",  yml, npm);
      MICROCI_TPL(true,  "plantuml",        ".microCI.yml",  yml, plantuml);
      MICROCI_TPL(true,  "pikchr",          ".microCI.yml",  yml, pikchr);
      MICROCI_TPL(true,  "clang-format",    ".microCI.yml",  yml, clang_format);
      MICROCI_TPL(false, "clang-format",    ".clang-format", yml, clang_format_config);
      MICROCI_TPL(true,  "beamer",          ".microCI.yml",  yml, beamer);
      MICROCI_TPL(true,  "fetch",           ".microCI.yml",  yml, fetch);
      MICROCI_TPL(true,  "minio",           ".microCI.yml",  yml, minio);
      MICROCI_TPL(true,  "flawfinder",      ".microCI.yml",  yml, flawfinder);
      MICROCI_TPL(true,  "docker_build",    ".microCI.yml",  yml, docker_build);
      MICROCI_TPL(true,  "pandoc",          ".microCI.yml",  yml, pandoc);
      MICROCI_TPL(true,  "template",        ".microCI.yml",  yml, template);
      // clang-format on
#undef MICROCI_TPL

      bool isNewPluginFound = false;
      for (const auto &[type, tpl] : templates) {
        if (newType == type) {
          isNewPluginFound = true;

          ofstream out;
          auto fileName = tpl.fileName;

          // Caso seja utilizado algum path diferente do default
          if (fileName == ".microCI.yml") {
            fileName = yamlFileName;
          }

          auto folderPos = fileName.find_last_of("/");
          if (folderPos != string::npos) {
            auto folderName = fileName.substr(0, folderPos);
            spdlog::debug(_("Creating folder '{}'"), folderName);
            filesystem::create_directories(folderName);
          }

          if (!tpl.appendIfExists and filesystem::exists(fileName)) {
            spdlog::debug(_("File '{}' already exists"), fileName);
            continue;
          } else if (tpl.appendIfExists and filesystem::exists(fileName)) {
            spdlog::debug(_("The file '{}' was edited from the template"), fileName);
            string step{(char *)tpl.fileContent, tpl.fileSize};
            step.erase(0, step.find("steps:") + 7);
            out.open(fileName, ios_base::app);
            out << "\n# --- PLEASE MERGE THE CONTENT BELOW TO YOUR RECIPE ---\n";
            out << step;
          } else {
            spdlog::debug(_("The file '{}' was created from the template"), fileName);
            out.open(fileName);
            out.write((char *)tpl.fileContent, tpl.fileSize);
          }
        }
      }

      if (isNewPluginFound) {
        // All done
        return 0;
      }
      spdlog::error(_("Invalid plugin type: {}"), newType);
      for (auto it = templates.begin(), end = templates.end(); it != end; it = templates.upper_bound(it->first)) {
        spdlog::debug(_("Use: microCI --new {}"), it->first);
      }
      return 1;
    }

    if (!filesystem::exists(yamlFileName)) {
      auto msg = fmt::format(_("The input file '{}' was not found"), yamlFileName);
      spdlog::error(msg);
      cout << fmt::format("echo '{}'\n", msg);
      return 1;
    }

    auto onlyStep = string{};
    if ((cmdl({"-O", "--only"}) >> onlyStep)) {
      uCI.SetOnlyStep(onlyStep);
    }

    if (!uCI.ReadConfig(yamlFileName)) {
      cout << microci::banner() << endl;
      auto msg = fmt::format(_("Failure reading the file '{}'"), yamlFileName);
      spdlog::error(msg);
      cout << fmt::format("echo '{}'\n", msg);
      return 1;
    }

    // Generate activity diagram and exit
    if (cmdl[{"-A", "--activity-diagram"}]) {
      cout << uCI.ActivityDiagram(yamlFileName) << endl;
      return 0;
    }

    if (cmdl[{"-T", "--test-config"}]) {
      return uCI.IsValid() ? 0 : 1;
    }

    if (cmdl[{"-U", "--update-db"}]) {
      if (uCI.IsValid() and filesystem::exists("/opt/microCI/db.json")) {
        json dbJson;
        {
          ifstream jsonFile("/opt/microCI/db.json");
          jsonFile >> dbJson;
        }
        auto CI = YAML::LoadFile(yamlFileName);

        string pwd = filesystem::absolute(yamlFileName).parent_path().lexically_normal();
        if (pwd.at(pwd.size() - 1) == '/') {
          pwd.erase(pwd.size() - 1);  // remove a barra no final
        }

        auto gitRemoteOrigin = string{};
        auto gitConfigFilename = pwd + "/.git/config";
        if (filesystem::exists(gitConfigFilename)) {
          ini::IniFile gitConfigIni;
          gitConfigIni.load(gitConfigFilename);
          gitRemoteOrigin = gitConfigIni["remote \"origin\""]["url"].as<string>();
        }

        spdlog::debug(_("PWD: {}"), pwd);
        spdlog::debug(_("Git config: {}"), gitConfigFilename);
        spdlog::debug(_("Git origin: {}"), gitRemoteOrigin);

        auto pwdRepoId = string{"_"};  // para evitar que a chave comece com número

        EVP_MD_CTX *mdctx;
        unsigned char *md5_digest;
        unsigned int md5_digest_len = EVP_MD_size(EVP_md5());
        mdctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
        EVP_DigestUpdate(mdctx, pwd.c_str(), pwd.size());
        md5_digest = (unsigned char *)OPENSSL_malloc(md5_digest_len);
        EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
        EVP_MD_CTX_free(mdctx);

        static const char hexchars[] = "0123456789abcdef";
        for (unsigned int i = 0; i < md5_digest_len; ++i) {
          unsigned char b = md5_digest[i];
          char hex[3];
          hex[0] = hexchars[b >> 4];
          hex[1] = hexchars[b & 0xF];
          hex[2] = 0;
          pwdRepoId.append(hex);
        }
        pwdRepoId = pwdRepoId.substr(0, 7);

        dbJson["repos"][pwdRepoId]["path"] = pwd;
        dbJson["repos"][pwdRepoId]["origin"] = gitRemoteOrigin;

        size_t stepNo = 0;
        for (auto step : CI["steps"]) {
          spdlog::debug("{} {}", stepNo, step["name"].as<string>());
          dbJson["repos"][pwdRepoId]["steps"][stepNo]["name"] = step["name"].as<string>();
          dbJson["repos"][pwdRepoId]["steps"][stepNo]["plugin"] = step["plugin"]["name"].as<string>();
          dbJson["repos"][pwdRepoId]["steps"][stepNo]["only"] = bool(step["only"]);
          if (step["description"]) {
            dbJson["repos"][pwdRepoId]["steps"][stepNo]["description"] = step["description"].as<string>();
          }
          ++stepNo;
        }

        {
          ofstream jsonFile("/opt/microCI/db.json");
          jsonFile << dbJson.dump(2);
        }
      }
      return 0;
    }

    cout << uCI.ToString();
  } catch (invalid_argument &e) {
    spdlog::error(e.what());
    return 1;
  } catch (runtime_error &e) {
    spdlog::error(e.what());
    return 1;
  } catch (...) {
    spdlog::error(_("Unknown exception"));
    return 1;
  }
  return 0;
}
