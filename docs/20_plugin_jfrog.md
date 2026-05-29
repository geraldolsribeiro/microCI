# 🚀 The **jfrog** Plugin

## Overview

The **jfrog** plugin provides seamless integration with JFrog's powerful artifact management ecosystem, allowing users to interact directly with JFrog Artifactory and other repository managers within a **microCI** pipeline. It transforms complex, multi-step deployment processes—such as building artifacts, pushing dependencies, and validating versions—into simple, declarative steps in your CI/CD configuration. By centralizing artifact storage and version control, this plugin ensures that all build outputs are traceable, secure, and readily available for consumption by downstream services.

## Features

The `jfrog` plugin provides capabilities focused on robust artifact lifecycle management:

### 🏛️ Repository Manager Integration
**Description:** This feature allows the CI pipeline to treat external repository managers (like Artifactory or Nexus) as first-class citizens. Users can upload built artifacts, download necessary dependencies, and manage package versions directly from **microCI**. The plugin abstracts away complex CLI calls (such as `jf rt`) into simple, declarative YAML definitions.
**Use Case:** Essential for any dependency management system. A typical workflow involves building a JAR/WAR file, using the **jfrog** plugin to upload it, and then having subsequent stages pull that validated artifact for deployment or integration testing.

### 📦 Artifactory Manager Integration
**Description:** Dedicated management of JFrog Artifactory, enabling fine-grained control over repositories (e.g., `maven-local`, `docker-virtual`). This includes operations like uploading multiple artifacts (`jf rt upload`), downloading specific versions, and managing repository keys or credentials securely within the CI context.
**Use Case:** Critical for microservices architectures. You can guarantee that every service deployment uses only known, validated artifacts pulled directly from a trusted Artifactory source, preventing "works on my machine" issues in production.

## Setup & Configuration

### ⚙️ Prerequisites (Credentials)

Before using the **jfrog** plugin, ensure your CI runner has access to valid JFrog credentials. Best practice dictates storing these as secure environment variables within the **microCI** platform:

*   `JFROG_USERNAME`: Your Artifactory username.
*   `JFROG_PASSWORD`: An API key or password generated for service accounts with minimum necessary write/read permissions.
*   *(Optional)* `JFROG_URL`: The base URL of your JFrog instance (e.g., `https://mycompany.jfrog.io`).

### 📄 Configuration Parameters

The plugin exposes parameters primarily through the `plugin` block, often allowing complex shell scripting or structured artifact definitions.

| Parameter | Type | Description | Required? | Example Value |
| :--- | :--- | :--- | :--- | :--- |
| `name` | String | The unique name of the plugin (must be `jfrog`). | Yes | `jfrog` |
| `bash` | Multi-line String | Allows running arbitrary JFrog CLI (`jf`) commands for maximum flexibility. This is used when structured operations are insufficient. | No | ```jf rt upload file.txt target/``` |
| `run_as` | String | Specifies the user context under which the plugin step will execute (e.g., `user`, `root`). | No | `user` |
| `envs` | Map<String, String> | Defines additional environment variables to be available during the execution of the underlying command. | No | `{"HOME": "/tmp/"}` |

### Example Configuration (`.microCI.yml`)

To push a set of build artifacts and associated files to a designated repository in Artifactory:

```yaml
steps:
    - name: "Build Artifacts"
      script: mvn clean install # Step that generates the artifact

    - name: "Publish Build Artifacts to Artifactory"
      plugin:
        name: jfrog
        # Use bash block for direct CLI interaction
        bash: |
          # Upload a specific file and create a structured path in artifactory
          jf rt upload target/my-service-1.0.jar my-repo/my-project/$BUILD_ID/
          # Example of syncing multiple files
          jf rt sync ./assets/ $ARTIFACTORY_REPO/web-site/

    - name: "Run Integration Tests"
      script: ./test_runner.sh
```

## Examples

This section demonstrates a complete, production-ready example using the `jfrog` plugin to execute a full artifact upload cycle.

```yaml
steps:
  - name: "Save artifact to jfrog artifact"
    description: "Description of this step"
    run_as: user
    envs:
      HOME: /tmp/
    plugin:
      name: jfrog
      bash: |
        # Copy files from workspace to the repository
        jf rt upload list.txt test-artifacts/folder/list.txt
```