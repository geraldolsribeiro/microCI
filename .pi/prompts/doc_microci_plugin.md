# Prompt Template: Plugin Documentation Generator (doc_microci_plugin)

**Objective:** Generate comprehensive, structured, and engaging documentation for a specific microCI plugin, ensuring it adheres to modern technical writing standards and clearly explains its functionality and integration steps.

**Parameters:**
*   `{{plugin name}}`: The descriptive name of the plugin (e.g., "JFrog Artifactory Plugin").
*   `{{markdown documentation}}`: The target Markdown file where the documentation content will be written/updated (e.g., `plugin_jfrog.md`).
*   `{{list of feature}}`: A list of key features, functions, or capabilities of the plugin, separated by commas or bullet points.

**Instructions:**
Reuse and refine previous documentation from {{markdown documentation}}.
Using the provided information about the `{{plugin name}}` and its core functionalities listed in `{{list of feature}}`, please draft/enhance the documentation for the file `{{markdown documentation}}`.
The output must be suitable for direct insertion into the specified Markdown file.

**Clarification policy for GPT-5:**
- If any required input is missing, ambiguous, or inconsistent, stop and ask for the missing parameter(s) before writing the documentation.
- Do not guess plugin behavior, configuration fields, or examples when the YAML configuration is incomplete.
- Ask for the minimum set of missing details needed to produce accurate documentation, such as:
  - the plugin name,
  - the target Markdown file,
  - the feature list,
  - the YAML configuration,
  - or the previous documentation to reuse.
- If the YAML configuration contains multiple possible interpretations, ask a clarifying question instead of inferring the intended one.

The resulting documentation should cover the following sections:
1.  **Overview:** A high-level description of what this plugin does and why a user would use it.
2.  **Features:** Detail each feature listed in `{{list of feature}}`. For each feature, provide a concise explanation and potential use case.
3.  **Setup & Configuration:** Clear step-by-step instructions on how to enable and configure the plugin within the microCI environment. Include necessary configuration parameters (e.g., API keys, credentials). Extract and document all params from {{yaml configuration}} in tabular format.
4.  **Examples:** Provide at least one complete code example demonstrating the plugin's usage in a CI/CD pipeline context. Use the yaml file {{yaml configuration}} as configuration example.

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

In the end write the generated documentation into file {{markdown documentation}}.
