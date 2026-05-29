# Prompt Template: Plugin Documentation Generator (doc_microci_plugin)

**Objective:** Generate comprehensive, structured, and engaging documentation for a specific microCI plugin, ensuring it adheres to modern technical writing standards and clearly explains its functionality and integration steps.

**Parameters:**
*   `{{plugin name}}`: The descriptive name of the plugin (e.g., "JFrog Artifactory Plugin").
*   `{{markdown filename}}`: The target Markdown file where the documentation content will be written/updated (e.g., `plugin_jfrog.md`).
*   `{{list of feature}}`: A list of key features, functions, or capabilities of the plugin, separated by commas or bullet points.

**Instructions:**
Reuse and refine previous documentation if possible.
Using the provided information about the `{{plugin name}}}` and its core functionalities listed in `{{list of feature}}`, please draft/enhance the documentation for the file `{{markdown filename}}`. The output must be suitable for direct insertion into the specified Markdown file.

The resulting documentation should cover the following sections:
1.  **Overview:** A high-level description of what this plugin does and why a user would use it.
2.  **Features:** Detail each feature listed in `{{list of feature}}`. For each feature, provide a concise explanation and potential use case.
3.  **Setup & Configuration:** Clear step-by-step instructions on how to enable and configure the plugin within the microCI environment. Include necessary configuration parameters (e.g., API keys, credentials). Extract and document all params from {{example}} in tabular format.
4.  **Examples:** Provide at least one complete code example demonstrating the plugin's usage in a CI/CD pipeline context. Use the yaml file {{example}} as configuration example.

The CI/CD pipeline file name is `.microCI.yml` with following structure:

```yaml
steps:
    - name: "Name of the step in the pipeline"
    - [common filds...]
    - plugin:
        - name: "plugin name"
        - [plugin specific filds]

    - name: "Another name of the step in the pipeline"
```

Render the word microCI as bold in markdown.

Ensure the tone is professional, highly technical, yet accessible to developers of varying skill levels. Use proper Markdown formatting (headings, lists, code blocks).
