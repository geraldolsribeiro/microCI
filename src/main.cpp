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
#include "DoxygenPluginStepParser.hpp"
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
#include "VHDLFormatPluginStepParser.hpp"

// Configuration templates
#include "new/bash.hpp"
#include "new/beamer.hpp"
#include "new/clang-format.hpp"
#include "new/clang-format_config.hpp"
#include "new/clang-tidy.hpp"
#include "new/cppcheck.hpp"
#include "new/docker_build.hpp"
#include "new/docmd.hpp"
#include "new/doxygen.hpp"
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
#include "new/vhdl-format.hpp"

// main class
#include "MicroCI.hpp"

using namespace microci;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto help() -> string {
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
  -n,--new doxygen         Create a documentation step
  -n,--new pandoc          Create a document conversion step
  -n,--new git_publish     Create a publish step
  -n,--new git_deploy      Create a production deploy step
  -n,--new plantuml        Create a diagram generation step
  -n,--new pikchr          Create a diagram generation step
  -n,--new clang-format    Create a code format step
  -n,--new vhdl-format     Create a code format step
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
void loadMicroCIEnviromentVariables(MicroCI &uCI, char **envp) {
  // Load environment variables
  for (char **env = envp; *env != nullptr; env++) {
    auto envStr = string{*env};
    if (envStr.size() > 7 and envStr.substr(0, 8) == "MICROCI_") {
      auto pos = envStr.find_first_of("=");
      EnvironmentVariable environmentVariable;
      environmentVariable.name = envStr.substr(0, pos);
      environmentVariable.value = envStr.substr(pos + 1);
      uCI.SetEnvironmentVariable(environmentVariable);
    }
  }
}

// ----------------------------------------------------------------------
// See https://docs.gitlab.com/ee/ci/variables/predefined_variables.html
// ----------------------------------------------------------------------
void loadGitlabEnvironmentVariables(MicroCI &uCI, char **envp) {
  set<string> allowedEnvironmentVariables = {
      "CHAT_CHANNEL",          // Pipeline	The Source chat channel that triggered the ChatOps command.
      "CHAT_INPUT",            // Pipeline	The additional arguments passed with the ChatOps command.
      "CHAT_USER_ID",          // Pipeline	The chat service’s user ID of the user who triggered the ChatOps command.
      "CI",                    // Pipeline	Available for all jobs executed in CI/CD. true when available.
      "CI_API_V4_URL",         // Pipeline	The GitLab API v4 root URL.
      "CI_API_GRAPHQL_URL",    // Pipeline	The GitLab API GraphQL root URL. Introduced in GitLab 15.11.
      "CI_BUILDS_DIR",         // Job-only	The top-level directory where builds are executed.
      "CI_COMMIT_AUTHOR",      // Pre-pipeline	The author of the commit in Name <email> format.
      "CI_COMMIT_BEFORE_SHA",  // Pre-pipeline	The previous latest commit present on a branch or tag. Is always
                               // 0000000000000000000000000000000000000000 for merge request pipelines, the first commit
                               // in pipelines for branches or tags, or when manually running a pipeline.
      "CI_COMMIT_BRANCH",  // Pre-pipeline	The commit branch name. Available in branch pipelines, including pipelines
                           // for the default branch. Not available in merge request pipelines or tag pipelines.
      "CI_COMMIT_DESCRIPTION",    // Pre-pipeline	The description of the commit. If the title is shorter than 100
                                  // characters, the message without the first line.
      "CI_COMMIT_MESSAGE",        // Pre-pipeline	The full commit message.
      "CI_COMMIT_REF_NAME",       // Pre-pipeline	The branch or tag name for which project is built.
      "CI_COMMIT_REF_PROTECTED",  // Pre-pipeline	true if the job is running for a protected reference, false otherwise.
      "CI_COMMIT_REF_SLUG",       // Pre-pipeline	CI_COMMIT_REF_NAME in lowercase, shortened to 63 bytes, and with
                             // everything except 0-9 and a-z replaced with -. No leading / trailing -. Use in URLs,
                             // host names and domain names.
      "CI_COMMIT_SHA",          // Pre-pipeline	The commit revision the project is built for.
      "CI_COMMIT_SHORT_SHA",    // Pre-pipeline	The first eight characters of CI_COMMIT_SHA.
      "CI_COMMIT_TAG",          // Pre-pipeline	The commit tag name. Available only in pipelines for tags.
      "CI_COMMIT_TAG_MESSAGE",  // Pre-pipeline	The commit tag message. Available only in pipelines for tags. Introduced
                                // in GitLab 15.5.
      "CI_COMMIT_TIMESTAMP",    // Pre-pipeline	The timestamp of the commit in the ISO 8601 format. For example,
                                // 2022-01-31T16:47:55Z. UTC by default.
      "CI_COMMIT_TITLE",        // Pre-pipeline	The title of the commit. The full first line of the message.
      "CI_CONCURRENT_ID",       // Job-only	The unique ID of build execution in a single executor.
      "CI_CONCURRENT_PROJECT_ID",  // Job-only	The unique ID of build execution in a single executor and project.
      "CI_CONFIG_PATH",            // Pipeline	The path to the CI/CD configuration file. Defaults to .gitlab-ci.yml.
      "CI_DEBUG_TRACE",            // Pipeline	true if debug logging (tracing) is enabled.
      "CI_DEBUG_SERVICES",         // Pipeline	true if service container logging is enabled. Introduced in GitLab 15.7.
                                   // Requires GitLab Runner 15.7.
      "CI_DEFAULT_BRANCH",         // Pipeline	The name of the project’s default branch.
      "CI_DEPENDENCY_PROXY_DIRECT_GROUP_IMAGE_PREFIX",  // Pipeline	The direct group image prefix for pulling images
                                                        // through the Dependency Proxy.
      "CI_DEPENDENCY_PROXY_GROUP_IMAGE_PREFIX",         // Pipeline	The top-level group image prefix for pulling images
                                                        // through the Dependency Proxy.
      "CI_DEPENDENCY_PROXY_PASSWORD",  // Pipeline	The password to pull images through the Dependency Proxy.
      "CI_DEPENDENCY_PROXY_SERVER",    // Pipeline	The server for logging in to the Dependency Proxy. This variable is
                                       // equivalent to $CI_SERVER_HOST:$CI_SERVER_PORT.
      "CI_DEPENDENCY_PROXY_USER",      // Pipeline	The username to pull images through the Dependency Proxy.
      "CI_DEPLOY_FREEZE",  // Pre-pipeline	Only available if the pipeline runs during a deploy freeze window. true when
                           // available.
      "CI_DEPLOY_PASSWORD",  // Job-only	The authentication password of the GitLab Deploy Token, if the project has
                             // one.
      "CI_DEPLOY_USER",      // Job-only	The authentication username of the GitLab Deploy Token, if the project has one.
      "CI_DISPOSABLE_ENVIRONMENT",  // Pipeline	Only available if the job is executed in a disposable environment
                                    // (something that is created only for this job and disposed of/destroyed after the
                                    // execution - all executors except shell and ssh). true when available.
      "CI_ENVIRONMENT_NAME",  // Pipeline	The name of the environment for this job. Available if environment:name is
                              // set.
      "CI_ENVIRONMENT_SLUG",  // Pipeline	The simplified version of the environment name, suitable for inclusion in DNS,
                              // URLs, Kubernetes labels, and so on. Available if environment:name is set. The slug is
                              // truncated to 24 characters. A random suffix is automatically added to uppercase
                              // environment names.
      "CI_ENVIRONMENT_URL",   // Pipeline	The URL of the environment for this job. Available if environment:url is set.
      "CI_ENVIRONMENT_ACTION",  // Pipeline	The action annotation specified for this job’s environment. Available if
                                // environment:action is set. Can be start, prepare, or stop.
      "CI_ENVIRONMENT_TIER",    // Pipeline	The deployment tier of the environment for this job.
      "CI_GITLAB_FIPS_MODE",    // Pre-pipeline	Only available if FIPS mode is enabled in the GitLab instance. true when
                                // available.
      "CI_HAS_OPEN_REQUIREMENTS",  // Pipeline	Only available if the pipeline’s project has an open requirement. true
                                   // when available.
      "CI_JOB_ID",                // Job-only	The internal ID of the job, unique across all jobs in the GitLab instance.
      "CI_JOB_IMAGE",             // Pipeline	The name of the Docker image running the job.
      "CI_JOB_JWT (Deprecated)",  // Pipeline	A RS256 JSON web token to authenticate with third party systems that
                                  // support JWT authentication, for example HashiCorp’s Vault. Deprecated in
                                  // GitLab 15.9 and scheduled to be removed in GitLab 17.0. Use ID tokens instead.
      "CI_JOB_JWT_V1 (Deprecated)",  // Pipeline	The same value as CI_JOB_JWT. Deprecated in GitLab 15.9 and scheduled
                                     // to be removed in GitLab 17.0. Use ID tokens instead.
      "CI_JOB_JWT_V2 (Deprecated)",  // Pipeline	A newly formatted RS256 JSON web token to increase compatibility.
                                     // Similar to CI_JOB_JWT, except the issuer (iss) claim is changed from gitlab.com
                                     // to https://gitlab.com, sub has changed from job_id to a string that contains the
                                     // project path, and an aud claim is added. The aud field is a constant value.
                                     // Trusting JWTs in multiple relying parties can lead to one RP sending a JWT to
                                     // another one and acting maliciously as a job. Deprecated in GitLab 15.9 and
                                     // scheduled to be removed in GitLab 17.0. Use ID tokens instead.
      "CI_JOB_MANUAL",               // Pipeline	Only available if the job was started manually. true when available.
      "CI_JOB_NAME",                 // Pipeline	The name of the job.
      "CI_JOB_NAME_SLUG",  // Pipeline	CI_JOB_NAME in lowercase, shortened to 63 bytes, and with everything except 0-9
                           // and a-z replaced with -. No leading / trailing -. Use in paths. Introduced in GitLab 15.4.
      "CI_JOB_STAGE",      // Pipeline	The name of the job’s stage.
      "CI_JOB_STATUS",   // Job-only	The status of the job as each runner stage is executed. Use with after_script. Can
                         // be success, failed, or canceled.
      "CI_JOB_TIMEOUT",  // Job-only	The job timeout, in seconds. Introduced in GitLab 15.7. Requires GitLab
                         // Runner 15.7.
      "CI_JOB_TOKEN",    // Job-only	A token to authenticate with certain API endpoints. The token is valid as long as
                         // the job is running.
      "CI_JOB_URL",      // Job-only	The job details URL.
      "CI_JOB_STARTED_AT",     // Job-only	The date and time when a job started, in ISO 8601 format. For example,
                               // 2022-01-31T16:47:55Z. UTC by default.
      "CI_KUBERNETES_ACTIVE",  // Pre-pipeline	Only available if the pipeline has a Kubernetes cluster available for
                               // deployments. true when available.
      "CI_NODE_INDEX",         // Pipeline	The index of the job in the job set. Only available if the job uses parallel.
      "CI_NODE_TOTAL",  // Pipeline	The total number of instances of this job running in parallel. Set to 1 if the job
                        // does not use parallel.
      "CI_OPEN_MERGE_REQUESTS",  // Pre-pipeline	A comma-separated list of up to four merge requests that use the
                                 // current branch and project as the merge request source. Only available in branch and
                                 // merge request pipelines if the branch has an associated merge request. For example,
                                 // gitlab-org/gitlab!333,gitlab-org/gitlab-foss!11.
      "CI_PAGES_DOMAIN",         // Pipeline	The configured domain that hosts GitLab Pages.
      "CI_PAGES_URL",            // Pipeline	The URL for a GitLab Pages site. Always a subdomain of CI_PAGES_DOMAIN.
      "CI_PIPELINE_ID",          // Job-only	The instance-level ID of the current pipeline. This ID is unique across all
                                 // projects on the GitLab instance.
      "CI_PIPELINE_IID",     // Pipeline	The project-level IID (internal ID) of the current pipeline. This ID is unique
                             // only in the current project.
      "CI_PIPELINE_SOURCE",  // Pre-pipeline	How the pipeline was triggered. The value can be one of the pipeline
                             // sources.
      "CI_PIPELINE_TRIGGERED",   // Pipeline	true if the job was triggered.
      "CI_PIPELINE_URL",         // Job-only	The URL for the pipeline details.
      "CI_PIPELINE_CREATED_AT",  // Pre-pipeline	The date and time when the pipeline was created, in ISO 8601 format.
                                 // For example, 2022-01-31T16:47:55Z. UTC by default.
      "CI_PIPELINE_NAME",        // Pre-pipeline	The pipeline name defined in workflow:name. Introduced in GitLab 16.3.
      "CI_PROJECT_DIR",   // Job-only	The full path the repository is cloned to, and where the job runs from. If the
                          // GitLab Runner builds_dir parameter is set, this variable is set relative to the value of
                          // builds_dir. For more information, see the Advanced GitLab Runner configuration.
      "CI_PROJECT_ID",    // Pipeline	The ID of the current project. This ID is unique across all projects on the GitLab
                          // instance.
      "CI_PROJECT_NAME",  // Pipeline	The name of the directory for the project. For example if the project URL is
                          // gitlab.example.com/group-name/project-1, CI_PROJECT_NAME is project-1.
      "CI_PROJECT_NAMESPACE",     // Pipeline	The project namespace (username or group name) of the job.
      "CI_PROJECT_NAMESPACE_ID",  // Pipeline	The project namespace ID of the job. Introduced in GitLab 15.7.
      "CI_PROJECT_PATH_SLUG",     // Pipeline	$CI_PROJECT_PATH in lowercase with characters that are not a-z or 0-9
                                  // replaced with - and shortened to 63 bytes. Use in URLs and domain names.
      "CI_PROJECT_PATH",          // Pipeline	The project namespace with the project name included.
      "CI_PROJECT_REPOSITORY_LANGUAGES",  // Pipeline	A comma-separated, lowercase list of the languages used in the
                                          // repository. For example ruby,javascript,html,css. The maximum number of
                                          // languages is limited to 5. An issue proposes to increase the limit.
      "CI_PROJECT_ROOT_NAMESPACE",  // Pipeline	The root project namespace (username or group name) of the job. For
                                    // example, if CI_PROJECT_NAMESPACE is root-group/child-group/grandchild-group,
                                    // CI_PROJECT_ROOT_NAMESPACE is root-group.
      "CI_PROJECT_TITLE",        // Pipeline	The human-readable project name as displayed in the GitLab web interface.
      "CI_PROJECT_DESCRIPTION",  // Pipeline	The project description as displayed in the GitLab web interface.
                                 // Introduced in GitLab 15.1.
      "CI_PROJECT_URL",          // Pipeline	The HTTP(S) address of the project.
      "CI_PROJECT_VISIBILITY",   // Pipeline	The project visibility. Can be internal, private, or public.
      "CI_PROJECT_CLASSIFICATION_LABEL",  // Pipeline	The project external authorization classification label.
      "CI_REGISTRY",  // Pipeline	Address of the container registry server, formatted as <host>[:<port>]. For example:
                      // registry.gitlab.example.com. Only available if the container registry is enabled for the GitLab
                      // instance.
      "CI_REGISTRY_IMAGE",  // Pipeline	Base address for the container registry to push, pull, or tag project’s images,
                            // formatted as <host>[:<port>]/<project_full_path>. For example:
                            // registry.gitlab.example.com/my_group/my_project. Image names must follow the container
                            // registry naming convention. Only available if the container registry is enabled for the
                            // project.
      "CI_REGISTRY_PASSWORD",  // Job-only	The password to push containers to the GitLab project’s container registry.
                               // Only available if the container registry is enabled for the project. This password
                               // value is the same as the CI_JOB_TOKEN and is valid only as long as the job is running.
                               // Use the CI_DEPLOY_PASSWORD for long-lived access to the registry
      "CI_REGISTRY_USER",  // Job-only	The username to push containers to the project’s GitLab container registry. Only
                           // available if the container registry is enabled for the project.
      "CI_RELEASE_DESCRIPTION",  // Pipeline	The description of the release. Available only on pipelines for tags.
                                 // Description length is limited to first 1024 characters. Introduced in GitLab 15.5.
      "CI_REPOSITORY_URL",  // Job-only	The full path to Git clone (HTTP) the repository with a CI/CD job token, in the
                            // format https://gitlab-ci-token:$CI_JOB_TOKEN@gitlab.example.com/my-group/my-project.git.
      "CI_RUNNER_DESCRIPTION",      // Job-only	The description of the runner.
      "CI_RUNNER_EXECUTABLE_ARCH",  // Job-only	The OS/architecture of the GitLab Runner executable. Might not be the
                                    // same as the environment of the executor.
      "CI_RUNNER_ID",               // Job-only	The unique ID of the runner being used.
      "CI_RUNNER_REVISION",         // Job-only	The revision of the runner running the job.
      "CI_RUNNER_SHORT_TOKEN",  // Job-only	The runner’s unique ID, used to authenticate new job requests. The token
                                // contains a prefix, and the first 17 characters are used.
      "CI_RUNNER_TAGS",         // Job-only	A comma-separated list of the runner tags.
      "CI_RUNNER_VERSION",      // Job-only	The version of the GitLab Runner running the job.
      "CI_SERVER_FQDN",         // Pipeline	The fully qualified domain name (FQDN) of the instance. For example
                                // gitlab.example.com:8080. Introduced in GitLab 16.10.
      "CI_SERVER_HOST",         // Pipeline	The host of the GitLab instance URL, without protocol or port. For example
                                // gitlab.example.com.
      "CI_SERVER_NAME",         // Pipeline	The name of CI/CD server that coordinates jobs.
      "CI_SERVER_PORT",  // Pipeline	The port of the GitLab instance URL, without host or protocol. For example 8080.
      "CI_SERVER_PROTOCOL",  // Pipeline	The protocol of the GitLab instance URL, without host or port. For example
                             // https.
      "CI_SERVER_SHELL_SSH_HOST",  // Pipeline	The SSH host of the GitLab instance, used for access to Git repositories
                                   // through SSH. For example gitlab.com. Introduced in GitLab 15.11.
      "CI_SERVER_SHELL_SSH_PORT",  // Pipeline	The SSH port of the GitLab instance, used for access to Git repositories
                                   // through SSH. For example 22. Introduced in GitLab 15.11.
      "CI_SERVER_REVISION",        // Pipeline	GitLab revision that schedules jobs.
      "CI_SERVER_TLS_CA_FILE",     // Pipeline	File containing the TLS CA certificate to verify the GitLab server when
                                   // tls-ca-file set in runner settings.
      "CI_SERVER_TLS_CERT_FILE",   // Pipeline	File containing the TLS certificate to verify the GitLab server when
                                   // tls-cert-file set in runner settings.
      "CI_SERVER_TLS_KEY_FILE",    // Pipeline	File containing the TLS key to verify the GitLab server when tls-key-file
                                   // set in runner settings.
      "CI_SERVER_URL",  // Pipeline	The base URL of the GitLab instance, including protocol and port. For example
                        // https://gitlab.example.com:8080.
      "CI_SERVER_VERSION_MAJOR",    // Pipeline	The major version of the GitLab instance. For example, if the GitLab
                                    // version is 17.2.1, the CI_SERVER_VERSION_MAJOR is 17.
      "CI_SERVER_VERSION_MINOR",    // Pipeline	The minor version of the GitLab instance. For example, if the GitLab
                                    // version is 17.2.1, the CI_SERVER_VERSION_MINOR is 2.
      "CI_SERVER_VERSION_PATCH",    // Pipeline	The patch version of the GitLab instance. For example, if the GitLab
                                    // version is 17.2.1, the CI_SERVER_VERSION_PATCH is 1.
      "CI_SERVER_VERSION",          // Pipeline	The full version of the GitLab instance.
      "CI_SERVER",                  // Job-only	Available for all jobs executed in CI/CD. yes when available.
      "CI_SHARED_ENVIRONMENT",      // Pipeline	Only available if the job is executed in a shared environment (something
                                    // that is persisted across CI/CD invocations, like the shell or ssh executor). true
                                    // when available.
      "CI_TEMPLATE_REGISTRY_HOST",  // Pipeline	The host of the registry used by CI/CD templates. Defaults to
                                    // registry.gitlab.com. Introduced in GitLab 15.3.
      "CI_TRIGGER_SHORT_TOKEN",  // Job-only	First 4 characters of the trigger token of the current job. Only available
                                 // if the pipeline was triggered with a trigger token. For example, for a trigger token
                                 // of glptt-dbf556605bcad4d9db3ec5fcef84f78f9b4fec28, CI_TRIGGER_SHORT_TOKEN would be
                                 // dbf5. Introduced in GitLab 17.0.
      "GITLAB_CI",               // Pipeline	Available for all jobs executed in CI/CD. true when available.
      "GITLAB_FEATURES",    // Pipeline	The comma-separated list of licensed features available for the GitLab instance
                            // and license.
      "GITLAB_USER_EMAIL",  // Pipeline	The email of the user who started the pipeline, unless the job is a manual job.
                            // In manual jobs, the value is the email of the user who started the job.
      "GITLAB_USER_ID",     // Pipeline	The numeric ID of the user who started the pipeline, unless the job is a manual
                            // job. In manual jobs, the value is the ID of the user who started the job.
      "GITLAB_USER_LOGIN",  // Pipeline	The unique username of the user who started the pipeline, unless the job is a
                            // manual job. In manual jobs, the value is the username of the user who started the job.
      "GITLAB_USER_NAME",   // Pipeline	The display name (user-defined Full name in the profile settings) of the user
                           // who started the pipeline, unless the job is a manual job. In manual jobs, the value is the
                           // name of the user who started the job.
      "KUBECONFIG",  // Pipeline	The path to the kubeconfig file with contexts for every shared agent connection. Only
                     // available when a GitLab agent is authorized to access the project.
      "TRIGGER_PAYLOAD",  // Pipeline	The webhook payload. Only available when a pipeline is triggered with a webhook.
  };

  for (char **env = envp; *env != nullptr; env++) {
    auto envStr = string{*env};
    auto pos = envStr.find_first_of("=");
    if (pos != string::npos) {
      EnvironmentVariable environmentVariable;
      environmentVariable.name = envStr.substr(0, pos);
      environmentVariable.value = envStr.substr(pos + 1);
      if (allowedEnvironmentVariables.count(environmentVariable.name) == 1) {
        uCI.SetEnvironmentVariable(environmentVariable);
      }
    }
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto main([[maybe_unused]] int argc, char **argv, char **envp) -> int {
  //{{{

  try {
    setlocale(LC_ALL, "");
    bindtextdomain("microci", nullptr);
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
    uCI.RegisterPlugin("doxygen", make_shared<DoxygenPluginStepParser>(&uCI));
    uCI.RegisterPlugin("beamer", make_shared<BeamerPluginStepParser>(&uCI));
    uCI.RegisterPlugin("plantuml", make_shared<PlantumlPluginStepParser>(&uCI));
    uCI.RegisterPlugin("pikchr", make_shared<PikchrPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_deploy", make_shared<GitDeployPluginStepParser>(&uCI));
    uCI.RegisterPlugin("git_publish", make_shared<GitPublishPluginStepParser>(&uCI));
    uCI.RegisterPlugin("mkdocs_material", make_shared<MkdocsMaterialPluginStepParser>(&uCI));
    uCI.RegisterPlugin("cppcheck", make_shared<CppCheckPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-tidy", make_shared<ClangTidyPluginStepParser>(&uCI));
    uCI.RegisterPlugin("clang-format", make_shared<ClangFormatPluginStepParser>(&uCI));
    uCI.RegisterPlugin("vhdl-format", make_shared<VHDLFormatPluginStepParser>(&uCI));
    uCI.RegisterPlugin("fetch", make_shared<FetchPluginStepParser>(&uCI));
    uCI.RegisterPlugin("minio", make_shared<MinioPluginStepParser>(&uCI));
    uCI.RegisterPlugin("flawfinder", make_shared<FlawfinderPluginStepParser>(&uCI));
    uCI.RegisterPlugin("docker_build", make_shared<DocmdPluginStepParser>(&uCI));
    uCI.RegisterPlugin("pandoc", make_shared<PandocPluginStepParser>(&uCI));
    uCI.RegisterPlugin("template", make_shared<TemplatePluginStepParser>(&uCI));

    loadMicroCIEnviromentVariables(uCI, envp);
    loadGitlabEnvironmentVariables(uCI, envp);

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
      MICROCI_TPL(true,  "doxygen",         ".microCI.yml",  yml, doxygen);
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
      MICROCI_TPL(true,  "vhdl-format",     ".microCI.yml",  yml, vhdl_format);
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
        EVP_DigestInit_ex(mdctx, EVP_md5(), nullptr);
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
