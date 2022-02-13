# Documentação do projeto

```bash
microCI --create mkdocs_material # Cria arquivo de configuração
vim .microCI.yml                 # Ajusta o arquivo de configuração
microCI | bash                   # Executa os passos
```

```yaml
steps:
  - name: "Construir documentação em formato HTML"
    description: "Documentação do projetos"
    plugin:
      name: "mkdocs_material"
      action: "build"
```

