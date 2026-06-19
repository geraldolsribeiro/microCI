// MIT License
//
// Copyright (C) 2022-2026 Geraldo Luis da Silva Ribeiro
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

#include "FetchPluginStepParser.hpp"

#include <fstream>

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void FetchPluginStepParser::Parse(const YAML::Node &step) {
  auto data = mMicroCI->DefaultDataTemplate();
  // data = parseRunAs(step, data, "user");
  data                     = parseRunAs(step, data, "root");  // From new version of bitnami/git
  data                     = parseNetwork(step, data, "host");
  data["STEP_NAME"]        = stepName(step);
  data["FUNCTION_NAME"]    = sanitizeName(stepName(step));
  data["STEP_DESCRIPTION"] = stepDescription(step, "Fetch external files used by the project");
  data["DOCKER_IMAGE"]     = stepDockerImage(step, "bitnamilegacy/git:latest");

  auto volumes             = parseVolumes(step);
  auto envs                = parseEnvs(step);
  tie(data, volumes, envs) = parseSsh(step, data, volumes, envs);

  // Suppress "Welcome to the Bitnami git container"
  EnvironmentVariable bitnamiDisableWelcomeMessage;
  bitnamiDisableWelcomeMessage.name  = "DISABLE_WELCOME_MESSAGE";
  bitnamiDisableWelcomeMessage.value = "true";
  envs.insert(bitnamiDisableWelcomeMessage);

  if (step["plugin"]["items"] && step["plugin"]["items"].IsSequence()) {
    auto defaultTarget = step["plugin"]["target"].as<std::string>("include/");

    beginFunction(data, envs);
    prepareRunDocker(data, envs, volumes);
    mMicroCI->Script() << inja::render(R"( \
        /bin/bash -c "cd {{ WORKSPACE }})",
                                       data);
    copySshIfAvailable(step, data);

    for (const auto &item : step["plugin"]["items"]) {
      auto gitTag     = item["tag"].as<std::string>("master");
      data["GIT_TAG"] = gitTag;
      data["TARGET"]  = item["target"].as<std::string>(defaultTarget);
      mMicroCI->Script() << inja::render(
          R"( \
           && mkdir -p {{ TARGET }})",
          data);

      if (item["github"]) {
        auto gitRemote = std::string{};
        std::stringstream ss(item["github"].as<std::string>());
        ss >> gitRemote >> gitTag;
        data["GIT_TAG"] = gitTag;

        // URL examples
        // https://github.com/User/repo/tarball/master
        // https://github.com/User/repo/archive/master.tar.gz
        // https://github.com/User/repo/archive/refs/tags/v0.1.2.tar.gz
        // https://github.com/User/repo/archive/refs/heads/master.zip
        // https://github.com/User/repo/archive/refs/heads/release/v0.1.1.zip

        if (gitTag == "master" or gitTag == "main") {
          gitRemote = "https://github.com/" + gitRemote + "/archive/" + gitTag + ".tar.gz";
        } else {
          gitRemote = "https://github.com/" + gitRemote + "/archive/refs/tags/" + gitTag + ".tar.gz";
        }

        if (item["offline"]) {
          gitRemote = "file://" + item["offline"].as<std::string>();
        }

        data["GIT_REMOTE"]       = gitRemote;
        data["FILES"]            = "";  // All files
        data["STRIP_COMPONENTS"] = " --strip-components=1";
        // data["STRIP_COMPONENTS"] = "";

        mMicroCI->Script() << inja::render(
            R"( \
           && curl -s -fSL -R -J --clobber {{ GIT_REMOTE }} \
             | tar -C {{ TARGET }}{{ STRIP_COMPONENTS }} -vzxf - {{ FILES }}2>&1)",
            data);

      } else if (item["git_archive"]) {
        std::string gitRemote;
        if (item["offline"]) {
          data["GIT_REMOTE"] = gitRemote = item["offline"].as<std::string>();
        } else {
          data["GIT_REMOTE"] = gitRemote = item["git_archive"].as<std::string>();
        }
        bool isGithubURL   = gitRemote.find("github.com") != std::string::npos;
        bool isDotGitEnded = gitRemote.substr(gitRemote.size() - 4) == ".git";

        auto files = std::string{};
        if (isGithubURL) {
          std::string repoName;
          if (isDotGitEnded) {
            // https://github.com/User/repo.git
            repoName = gitRemote.substr(gitRemote.find_last_of('/') + 1);
            repoName.erase(repoName.find(".git"));
          } else {
            // https://github.com/User/repo/archive/master.tar.gz
            repoName = gitRemote.substr(0, gitRemote.find("/archive/"));
            repoName = repoName.substr(repoName.find_last_of('/') + 1);
          }

          for (const auto &f : item["files"]) {
            // Single quotes to avoid expansion
            files += fmt::format("'{}-{}/{}' ", repoName, gitTag, f.as<std::string>());
          }
        } else {
          for (const auto &f : item["files"]) {
            // Single quotes to avoid expansion
            files += fmt::format("'{}' ", f.as<std::string>());
          }
        }

        if (files.empty()) {
          throw std::runtime_error("It is mandatory to specify a file list");
        }

        data["FILES"] = files;

        if (isGithubURL) {
          if (item["strip-components"]) {
            data["STRIP_COMPONENTS"] = " --strip-components=" + std::to_string(item["strip-components"].as<int>() + 1);
          } else {
            data["STRIP_COMPONENTS"] = " --strip-components=1";
          }
        } else {
          if (item["strip-components"]) {
            data["STRIP_COMPONENTS"] = " --strip-components=" + item["strip-components"].as<std::string>();
          } else {
            data["STRIP_COMPONENTS"] = "";
          }
        }

        if (isGithubURL and item["token"]) {
          // https://<personal_token>:@github.com/<your_repo>/archive/main.tar.gz
          gitRemote.insert(gitRemote.find("github.com"), item["token"].as<std::string>() + "@");
          data["GIT_REMOTE"] = gitRemote;
        }

        if (isGithubURL and isDotGitEnded) {
          // From: https://github.com/User/repo.git
          // To: https://github.com/User/repo/archive/master.tar.gz
          data["GIT_REMOTE"] = gitRemote.substr(0, gitRemote.size() - 4) + "/archive/" + gitTag + ".tar.gz";
        }

        if (isGithubURL) {
          mMicroCI->Script() << inja::render(
              R"( \
           && curl -s -fSL -R -J --clobber {{ GIT_REMOTE }} \
             | tar -C {{ TARGET }}{{ STRIP_COMPONENTS }} -vzxf - {{ FILES }}2>&1)",
              data);
        } else if (item["offline"]) {
          mMicroCI->Script() << inja::render(
              R"( \
           && tar -C {{ TARGET }}{{ STRIP_COMPONENTS }} -vxf {{ GIT_REMOTE }} {{ FILES }} 2>&1)",
              data);
        } else {
          mMicroCI->Script() << inja::render(
              R"( \
           && git archive --format=tar --remote={{ GIT_REMOTE }} {{ GIT_TAG }} {{ FILES }} \
             | tar -C {{ TARGET }}{{ STRIP_COMPONENTS }} -vxf - 2>&1)",
              data);
        }

      } else if (item["url"]) {
        data["TARGET"] = item["target"].as<std::string>(defaultTarget);
        data["URL"]    = item["url"].as<std::string>();
        mMicroCI->Script() << inja::render(
            R"( \
           && pushd {{ TARGET }} \
           && curl -fSL -R -J --clobber -O {{ URL }} 2>&1 \
           && popd)",
            data);
      }

      mMicroCI->Script() << inja::render(
          R"( \
           ; chown $(id -u):$(id -g) -Rv {{ TARGET }})",
          data);
    }
    data["TARGET"] = defaultTarget;
    mMicroCI->Script() << inja::render(
        R"( \
           ; echo 'Default target chown' \
           ; chown $(id -u):$(id -g) -Rv {{ TARGET }})",
        data);
    mMicroCI->Script() << "\"\n";
    endFunction(data);
  }
}
}  // namespace microci
