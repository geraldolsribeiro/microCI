[//]: <> (Documentation generated by intmain_docmd)
# pandoc plugin


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

