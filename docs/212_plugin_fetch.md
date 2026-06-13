# ⚙️ Fetch plugin

## Overview

The **microCI** `fetch` plugin downloads external files and archives into the build workspace. It is useful when a pipeline needs assets that live outside the repository.

Use this plugin when your workflow depends on external artifacts that are not committed to the repository.

## Features

- **Download a single file from a URL**  
  Retrieve raw files directly from HTTP(S) endpoints. Useful for vendoring headers or configuration files.

- **Extract files from a Git archive**  
  Download a repository archive and extract only the required paths. Useful when you need a small subset of a large repository.

- **Fetch from GitHub repositories**  
  Download source content from GitHub using a branch, tag, or offline archive. Useful for reproducible dependency retrieval.

- **Support private repositories**  
  Authenticate with SSH keys or tokens when downloading protected content.

- **Offline fallback support**  
  Use a local archive when network access is unavailable. Useful for air-gapped or cached builds.

- **Path filtering and globbing**  
  Extract specific files using path patterns such as `include/*.h` or `lib/*.so`.

## Setup and configuration

### Enable the plugin

Add a pipeline step in `.microCI.yml` and configure the `fetch` plugin under `plugin`.

### Configuration parameters

| Parameter | Type | Required | Description |
|---|---:|---:|---|
| `steps[].name` | string | yes | Name of the pipeline step. |
| `steps[].ssh.copy_from` | string | no | Source path for SSH credentials on the host machine. |
| `steps[].ssh.copy_to` | string | no | Destination path inside the container where SSH credentials are injected. |
| `steps[].plugin.name` | string | yes | Must be set to `fetch`. |
| `steps[].plugin.target` | string | no | Default destination directory for fetched files. Can be overridden per item. |
| `steps[].plugin.items` | array | yes | List of files or archives to fetch. |
| `items[].git_archive` | string | conditional | Git repository URL to archive and extract. |
| `items[].url` | string | conditional | Direct URL to a single file. |
| `items[].github` | string | conditional | GitHub repository specifier in the form `owner/repo ref`. |
| `items[].target` | string | no | Destination directory for the specific item. |
| `items[].tag` | string | no | Tag, branch, commit hash, or `HEAD` for Git archive downloads. |
| `items[].strip-components` | integer | no | Removes leading path components from extracted archive entries. |
| `items[].files` | array | conditional | List of files or patterns to extract from an archive. |
| `items[].token` | string | no | Authentication token for private repositories. |
| `items[].offline` | string | no | Local archive used as fallback when network access is unavailable. |

### Configuration notes

- Use `ssh.copy_from` and `ssh.copy_to` when fetching from SSH-based Git repositories.
- Use `token` for HTTPS-authenticated access to private repositories.
- Use `offline` to support deterministic or disconnected builds.
- Set `strip-components` when the archive contains nested directories you want to flatten.

## Example

```yaml
steps:
  - name: "Download external files into the project"
    description: "Retrieve build dependencies from external sources."
    ssh:
      copy_from: "${HOME}/.ssh"
      copy_to: "/home/bitnami/.ssh"
    plugin:
      name: "fetch"
      target: include
      items:
        - git_archive: https://github.com/geraldolsribeiro/microCI.git
          target: /tmp/
          tag: master
          strip-components: 1
          files:
            - test/help.txt
        - git_archive: https://github.com/User/repo/archive/master.tar.gz
          target: /tmp/
          token: personal_token
          files:
            - README.md
        - git_archive: https://github.com/User/repo/archive/master.tar.gz
          offline: /tmp/repo.tar.gz
          target: /tmp/
          token: personal_token
          files:
            - README.md
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/include/
          tag: HEAD
          files:
            - README.md
            - include/*.h
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/lib/
          files:
            - lib/*.so
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/lib/
          strip-components: 2
          files:
            - path1/path2/lib/*.so
        - url: https://raw.githubusercontent.com/adishavit/argh/master/argh.h
          target: /tmp/include
        - url: https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
        - url: https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp
        - github: adishavit/argh master
        - github: adishavit/argh v1.3.2
          target: /tmp/
        - github: adishavit/argh v1.3.2
          offline: /tmp/repo2.tar.gz
```

## Usage guidance

- Prefer `url` for single-file dependencies.
- Prefer `git_archive` when only a subset of repository files is needed.
- Prefer `github` for concise GitHub-based dependency references.
- Keep fetched artifacts minimal to reduce build time and improve supply-chain control.
