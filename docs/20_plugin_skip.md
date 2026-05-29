# Plugin Documentation: Skip Step (`skip`)

## Overview

The `skip` plugin is a utility designed for managing and controlling the execution flow of your **microCI** pipeline steps. Its primary function is to temporarily or conditionally disable a specific step without needing to comment out or restructure the entire build definition. This makes it invaluable for debugging, testing partial pipelines, running smoke tests, or bypassing expensive/time-consuming stages in controlled environments.

If a step contains the `plugin: name: skip` directive, **microCI** will detect this and gracefully exit that specific step without running any associated commands or validations, allowing the pipeline to continue processing subsequent steps if configured otherwise.

## Features

*   **Skip Temporarily:** Allows developers to bypass an entire build step instantly using a simple plugin declaration (`name: skip`). This is ideal for isolating failures or testing adjacent components without needing to comment out large blocks of YAML code.
*   **Non-Intrusive Debugging:** Facilitates debugging by allowing users to run the pipeline up to a specific point and then skip the problematic section, thereby validating preceding steps in isolation.

## Setup & Configuration

The `skip` plugin is extremely simple to implement, as its core functionality is derived from its presence rather than complex parameters. You simply need to declare it within a step's `plugin:` block.

**Required Parameters:**
None. The plugin relies solely on the presence of the `name: skip` directive.

**Configuration Example (`@new/skip.yml`):**

The configuration is applied directly in the pipeline definition:

```yaml
# To ignore a step entirely, simply declare the plugin with no specific parameters.
plugin:
  name: skip 
```

## Examples

### Example 1: Completely Bypassing an Entire Step

Use this example to show how to ignore a preparatory or validation step without affecting other parts of the build process.

**`steps:` Block:**

```yaml
steps:
    # This step is designed to be ignored during rapid testing.
    - name: "Ignored setup step"
      description: "This step should be ignored temporarily."
      plugin:
        name: skip # <-- Execution will stop here for this step

    # This subsequent step will execute normally, proving the previous step was skipped.
    - name: "Standard Build Step"
      docker: "ubuntu:latest"
      script: |
        echo "This message confirms that 'Ignored setup step' was successfully bypassed."
```

### Example 2: Skipping a Complex Block for Testing

When testing the functionality of steps *after* a complex build process, you might want to skip the entire build/install block if artifacts are already known to be present.

**`steps:` Block (from `@new/skip.yml`):**

```yaml
steps:
    # ... other setup steps ...

    - name: "Another ignored step - build static version of microCI"
      description: "This complex block is skipped for testing purposes."
      plugin:
        name: skip # <-- Bypassing the full compile and test cycle here.
```

***
*Note: The `skip` plugin does not require any credentials, API keys, or environment variables.*
***