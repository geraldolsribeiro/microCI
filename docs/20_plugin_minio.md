# ☁️ The **MinIO** Plugin

## Overview

The **minio** plugin provides robust and native integration for interacting with S3-compatible object storage systems, such as MinIO or Amazon S3. It allows developers to treat cloud object storage buckets as a first-class resource directly within the **microCI** pipeline. By integrating reliable mechanisms for uploading build artifacts, fetching necessary dependencies, and managing state data, this plugin ensures that all components are stored in a standardized, versioned, and highly available manner before deployment.

## Features

The core capability of this plugin is providing seamless connectivity to S3-compatible storage:

### 🌐 S3-Compatible Object Storage System Integration
**Description:** This feature enables the CI pipeline to execute standard object storage operations—including listing contents (`ls`), uploading files/directories (`cp`), and downloading objects—using the industry-standard `mc` (MinIO Client) command structure. The plugin encapsulates these complex CLI calls, making them easily declarative in YAML.
**Use Case:** Essential for cloud-native deployments. Instead of relying on local file systems, pipelines can reliably persist all build outputs (logs, models, configuration files, binaries) into a dedicated storage bucket, ensuring data integrity and traceability regardless of the execution environment.

## Setup & Configuration

### ⚙️ Prerequisites (Authentication)

Before using the **minio** plugin, ensure that your CI runner has access to valid cloud credentials. Best practice is to configure these as secure environment variables within the **microCI** platform:

*   `MINIO_ENDPOINT`: The endpoint URL of the MinIO server (e.g., `http://storage.mycompany.com:9000`).
*   `MINIO_ACCESSKEY`: Your primary access key.
*   `MINIO_SECRETKEY`: Your secret key corresponding to the access key.

### 📄 Configuration Parameters

Configuration is managed primarily through a shell script block (`bash`) within the `plugin` definition, leveraging the powerful and flexible MinIO CLI commands (`mc`).

| Parameter | Type | Description | Required? | Example Value |
| :--- | :--- | :--- | :--- | :--- |
| `name` | String | The unique name of the plugin (must be `minio`). | Yes | `minio` |
| `bash` | Multi-line String | Allows running arbitrary MinIO Client (`mc`) commands. This is the primary method for defining complex object storage operations like copy or list. | No | ```mc ls mybucket/path/``` |
| `run_as` | String | Specifies the user context under which the plugin step will execute (e.g., `user`). | No | `user` |
| `envs` | Map<String, String> | Defines additional environment variables to be available during the execution of the underlying command (e.g., passing bucket credentials). | No | `{"MINIO_ACCESSKEY": "..."}` |

### Example Configuration (`.microCI.yml`)

To demonstrate listing objects and uploading artifacts:

```yaml
steps:
    - name: "Prepare Artifacts"
      script: make build # Step that generates artifact files (Makefile, etc.)

    - name: "Upload Build Artifacts to MinIO Bucket"
      plugin:
        name: minio
        bash: |
          # 1. List objects in a target bucket for verification
          mc ls myci-artifacts/build-reports/
          # 2. Upload the necessary artifacts from workspace (Makefile)
          mc cp Makefile myci-artifacts/$BUILD_ID/
          # 3. Example: Sync an entire directory recursively
          mc sync ./docs $ARTIFACTORY_BUCKET/documentation/

    - name: "Fetch Configuration From MinIO"
      plugin:
        name: minio
        bash: |
          # Download a required config file from the bucket to local workspace
          mc cp myci-artifacts/$BUILD_ID/config.json .
```

## Examples

This section provides an example of using the **minio** plugin in a production workflow, demonstrating how to list files, copy data to the bucket, and fetch data back into the workspace for subsequent steps.

```yaml
steps:
  - name: "Save artifact in bucket"
    description: "Description of this step"
    run_as: user
    envs:
      HOME: /tmp/
    plugin:
      name: minio
      bash: |
        # 1. List files in the bucket for verification
        mc ls microci/bucket_name
        # 2. Copy local artifacts from workspace to the remote bucket
        mc cp Makefile microci/bucket_name/
        # 3. Fetch an artifact from the bucket back into the current working directory (.)
        mc cp microci/bucket_name/image.png .
```