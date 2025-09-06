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

#include "PluginStepParser.hpp"

#include <spdlog/spdlog.h>

#include "MicroCI.hpp"

namespace microci {

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::IsValid() const -> bool { return mIsValid; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PluginStepParser::invalidConfigurationDetected() { mIsValid = false; }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseRunAs(const YAML::Node &step, const json &data, const string &defaultValue) const -> json {
  auto data_ = data;
  data_["RUN_AS"] = defaultValue;
  if (step["run_as"]) {
    data_["RUN_AS"] = step["run_as"].as<string>();
  }
  return data_;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseNetwork(const YAML::Node &step, const json &data, const string &defaultValue) const
    -> json {
  auto data_ = data;
  data_["DOCKER_NETWORK"] = defaultValue;
  if (step["network"]) {
    data_["DOCKER_NETWORK"] = step["network"].as<string>();
  }
  return data_;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::stepDockerImage(const YAML::Node &step, const string &image) const -> string {
  string dockerImage = mMicroCI->DefaultDockerImage();

  if (!image.empty()) {
    dockerImage = image;
    mMicroCI->AddDockerImage(image);
  }

  if (step["docker"]) {
    dockerImage = step["docker"].as<string>();
  }

  mMicroCI->AddDockerImage(dockerImage);
  return dockerImage;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseDevices(const YAML::Node &step, const json &data) const -> json {
  auto data_ = data;
  data_["DOCKER_DEVICES"] = json::array();

  if (step["devices"] and step["devices"].IsSequence()) {
    for (const auto &dev : step["devices"]) {
      data_["DOCKER_DEVICES"].push_back(dev.as<string>());
    }
  }
  return data_;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::stepDockerWorkspace(const YAML::Node &step, const string &workspace) const -> string {
  string dockerWorkspace = mMicroCI->DefaultWorkspace();

  if (!workspace.empty()) {
    dockerWorkspace = workspace;
  }

  if (step["workspace"]) {
    dockerWorkspace = step["workspace"].as<string>();
  }

  return dockerWorkspace;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseEnvs(const YAML::Node &step) const -> set<EnvironmentVariable> {
  auto ret = mMicroCI->DefaultEnvs();
  if (step["envs"] and step["envs"].IsMap()) {
    for (const auto &it : step["envs"]) {
      EnvironmentVariable env;
      env.name = it.first.as<string>();
      env.value = it.second.as<string>();
      ret.insert(env);
    }
  }
  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PluginStepParser::beginFunction(const json &data, const set<EnvironmentVariable> &envs) {
  mMicroCI->Script() << inja::render(R"(
# ----------------------------------------------------------------------
# {{ STEP_DESCRIPTION }}
# ----------------------------------------------------------------------
function step_{{ FUNCTION_NAME }}() {
  local SECONDS=0
  local MICROCI_STEP_NAME="{{ STEP_NAME }}"
  local MICROCI_STEP_DESCRIPTION="{{ STEP_DESCRIPTION }}"
  local MICROCI_GIT_ORIGIN=$( git config --get remote.origin.url || echo "GIT ORIGIN NOT FOUND" )
  local MICROCI_GIT_COMMIT_SHA=$( git rev-parse --short HEAD || echo "GIT COMMIT HASH NOT FOUND")
  local MICROCI_GIT_COMMIT_MSG=$( git show -s --format=%s )
  local MICROCI_STEP_STATUS=":ok:"
  local MICROCI_STEP_SKIP="{{ MICROCI_STEP_SKIP }}"
  local MICROCI_STEP_DURATION=$SECONDS

  # Make step line with 60 characters
  local title="$(( MICROCI_STEP_NUMBER + 1 )) ${MICROCI_STEP_NAME}.............................................................."
  local title=${title:0:60}

  echo -ne "{{CYAN}}${title}{{CLEAR}}: "
)",
                                     data);
  for (auto env : envs) {
    mMicroCI->Script() << fmt::format("  local {}=\"{}\"\n", env.name, env.value);
  }

  mMicroCI->Script() << inja::render(R"(
  {
    (
      set -e
)",
                                     data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PluginStepParser::endFunction(const json &data) {
  mMicroCI->Script() << inja::render(R"(
    )

    status=$?
    MICROCI_STEP_DURATION=$SECONDS
    echo "Status: ${status}"
    echo "Duration: ${MICROCI_STEP_DURATION}"
  } >> .microCI.log

  # Notification at the terminal
  if [ "${MICROCI_STEP_SKIP}" = "yes" ]
  then
    echo -e "{{BLUE}}SKIP{{CLEAR}}"
    setStepStatusSkipJson
  elif [ "${status}" = "0" ]
  then
    echo -e "{{GREEN}}OK{{CLEAR}}"
    setStepStatusOkJson
  else
    echo -e "{{RED}}FAILED{{CLEAR}}"
    setStepStatusFailJson
    echo "See the complete log at .microCI.log"
    tail -50 .microCI.log
    exit 1
  fi

  ((++MICROCI_STEP_NUMBER))
)",
                                     data);

  auto envs = mMicroCI->DefaultEnvs();
  auto webhookEnv = EnvironmentVariable{"MICROCI_DISCORD_WEBHOOK", ""};
  if (envs.count(webhookEnv)) {
    mMicroCI->Script() << inja::render(R"(
  # Notification via Discord
  # Use spycolor.com to get the color in decimal
  if [ "${status}" = "0" ]; then
    MICROCI_STEP_STATUS=":ok:"
    MICROCI_STEP_STATUS_COLOR=4382765
  else
    MICROCI_STEP_STATUS=":face_with_symbols_over_mouth:"
    MICROCI_STEP_STATUS_COLOR=16711680
  fi
  notify_discord
)",
                                       data);
  }

  mMicroCI->Script() << "}" << endl;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PluginStepParser::prepareRunDocker(const json &data, const set<EnvironmentVariable> &envs,
                                        const set<DockerVolume> &volumes) {
  mMicroCI->Script() << inja::render(R"(
      echo ""
      echo ""
      echo ""
      echo "Step: {{ STEP_NAME }}"
      # shellcheck disable=SC2140,SC2046
      docker run)",
                                     data);

  if (string{data["RUN_AS"]} != "root") {
    mMicroCI->Script() << inja::render(R"( \
        --user $(id -u):$(id -g))",
                                       data);
  }

  if (data.count("DOCKER_ENTRYPOINT")) {
    mMicroCI->Script() << inja::render(R"( \
        --entrypoint={{DOCKER_ENTRYPOINT}})",
                                       data);
  }

  if (data.count("DOCKER_DEVICES")) {
    mMicroCI->Script() << inja::render(R"( \
## for device in DOCKER_DEVICES
        --device {{ device }} \
## endfor)",
                                       data);
    // remove newline, backslash, and space from last entry
    mMicroCI->Script().seekp(-3, mMicroCI->Script().cur);
  }

  mMicroCI->Script() << inja::render(R"( \
        --interactive \
        --attach stdout \
        --attach stderr \
        --rm \
        --name microci_{{ FUNCTION_NAME }}_{{ RANDOM_8 }} \
        --network {{ DOCKER_NETWORK }} \
        --workdir {{ WORKSPACE }})",
                                     data);

  for (auto &env : envs) {
    mMicroCI->Script() << fmt::format(R"( \
        --env {}="{}")",
                                      env.name, env.value);
  }

  for (const auto &vol : volumes) {
    mMicroCI->Script() << fmt::format(R"( \
        --volume "{}":"{}":{})",
                                      vol.source, vol.destination, vol.mode);
  }

  mMicroCI->Script() << inja::render(R"( \
        "{{ DOCKER_IMAGE }}")",
                                     data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseSsh(const YAML::Node &step, const json &data, const set<DockerVolume> &volumes,
                                const set<EnvironmentVariable> &envs) const
    -> tuple<json, set<DockerVolume>, set<EnvironmentVariable>> {
  auto sshMountForCopy = string{"${HOME}/.ssh"};
  auto volumes_ = volumes;
  auto data_ = data;
  auto envs_ = envs;

  data_["SSH_COPY_TO"] = string{};
  data_["SSH_COPY_FROM"] = "/.microCI_ssh";

  if (step["ssh"]) {
    // the folder specified by copy_from will be mounted at /.microCI_ssh
    if (step["ssh"]["copy_from"]) {
      sshMountForCopy = step["ssh"]["copy_from"].as<string>();
    }

    if (step["ssh"]["copy_to"]) {
      data_["SSH_COPY_TO"] = step["ssh"]["copy_to"].as<string>();
    } else {
      auto gitSshCommandEnv = EnvironmentVariable{"GIT_SSH_COMMAND",
                                                  "ssh -i /.microCI_ssh/id_rsa"
                                                  " -F /dev/null"
                                                  " -o UserKnownHostsFile=/.microCI_ssh/known_hosts"};
      envs_.insert(gitSshCommandEnv);
    }

    // Temporary mounting for copy
    DockerVolume vol;
    vol.destination = "/.microCI_ssh";
    vol.source = sshMountForCopy;
    vol.mode = "ro";
    volumes_.insert(vol);
  }
  return {data_, volumes_, envs_};
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void PluginStepParser::copySshIfAvailable(const YAML::Node &step, const json &data) {
  if (!step["ssh"] or !step["ssh"]["copy_to"]) {
    return;
  }

  mMicroCI->Script() << inja::render(R"( \
           && mkdir -p {{ SSH_COPY_TO }} 2>&1 \
           && cp -Rv {{ SSH_COPY_FROM }} {{ SSH_COPY_TO }} 2>&1 \
           && chmod 700 {{ SSH_COPY_TO }}/ 2>&1 \
           && chmod 644 {{ SSH_COPY_TO }}/id_rsa.pub 2>&1 \
           && chmod 600 {{ SSH_COPY_TO }}/id_rsa 2>&1)",
                                     data);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto PluginStepParser::parseVolumes(const YAML::Node &step) const -> set<DockerVolume> {
  auto volumes = mMicroCI->DefaultVolumes();

  if (step["volumes"] && step["volumes"].IsSequence()) {
    for (const auto &volume : step["volumes"]) {
      DockerVolume vol;
      vol.destination = volume["destination"].as<string>();
      vol.source = volume["source"].as<string>();
      if (volume["mode"]) {
        vol.mode = volume["mode"].as<string>();
      } else {
        vol.mode = "ro";
      }
      volumes.insert(vol);
    }
  }

  return volumes;
}

}  // namespace microci
