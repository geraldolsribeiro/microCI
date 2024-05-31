[//]: <> (Documentação gerada com intmain_docmd)
# Example of use of the pikchr plugin

```yaml
steps:
  - name: "Build diagrams"
    description: "Build diagrams from textual description"
    plugin:
      name: "pikchr"
      source:
        - "docs/diagrams/*.pikchr"            # Search for all filename with pikchr extension
      type: svg                               # Can be svg, png or pdf
      output_folder: "docs/diagrams/"         # Folder for saving generated diagrams
```

