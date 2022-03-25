[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin plantuml

```yaml
steps:
  - name: "Gerar diagramas plantuml"
    description: "Descrição deste passo"
    plugin:
      name: "plantuml"
      source:
        - "src/**.cpp"         # Vasculha recursivamente os arquivos cpp
        - "docs/**.puml"
      type: svg                # svg, png ou pdf
      output: "/microci_workspace/docs/diagrams/" # Pasta onde serão salvos os arquivos
```

