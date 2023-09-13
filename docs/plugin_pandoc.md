[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin pandoc

```yaml
steps:
  - name: "Gerar PDF a partir do Markdown"
    description: "Descrição deste passo"
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

