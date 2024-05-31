[//]: <> (Documentação gerada com intmain_docmd)
# Example of using the pandoc plugin

```yaml
steps:
  - name: "Generate PDF from Markdown"
    description: "Description of this step"
    plugin:
      name: pandoc
      base_path: .
      inputs:
        - README.md
      output: README.pdf
      options:
        - --standalone
        - --toc
        - --number-sections
```

