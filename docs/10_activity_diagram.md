# Activity diagram

To generate activity diagrams for the current workflow use the option `--activity-diagram`:

Generate the Plantuml description file:

```bash
microCI --activity-diagram > docs/diagrams/microCI_activity_diagram.puml
```

Generate the images from the description file:

```yaml
- steps:
  - name: "Build diagrams with plantuml"
    description: "Build diagrams from textual description"
    plugin:
      name: "plantuml"
      source:
        - "docs/diagrams/*.puml"
      type: png
      config: "docs/diagrams/skinparams.iuml"
      output: "/microci_workspace/docs/diagrams"
```


There are two king of steps:

* **main** step: list of sequential steps executed on the **main** pipeline
* **only** step: alternative single step executed alone

The diagrams below are generated automatically by **microCI** based on
[`.microCI.yml`](https://raw.githubusercontent.com/geraldolsribeiro/microCI/refs/heads/master/.microCI.yml) file.

![Main pipeline](diagrams/microCI_activity_diagram.png)

![Alternative step](diagrams/microCI_activity_diagram_001.png)

![Alternative step](diagrams/microCI_activity_diagram_002.png)
