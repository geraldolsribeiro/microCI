[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin mkdocs_material

```yaml
---
steps:
  - name: "Construir documentação em formato HTML"
    description: "Documentação do projeto"
    plugin:
      name: "mkdocs_material"
      action: "build"
      # Para construir a documentação em outra pasta use
      # action: "build --site-dir nome_da_pasta
  - name: "Publicar HTML para repositório git"
    ssh:
      copy_from: "${HOME}/.ssh"
      copy_to: "/root/.ssh"
    plugin:
      name: "git_publish"
      git_url: "ssh://git@someurl.com/site.git"
      copy_to: "/deploy" # Parâmetro default
      copy_from: "site"  # Parâmetro default
      clean_before: true # Parâmetro default
  - name: "Servidor local na porta 8000 (Ctrl+C para finalizar)"
    description: "Executa servidor local para preview da documentação"
    only: "serve" # passo executado localmente para auxiliar na edição/preview
    network: host
    plugin:
      name: "mkdocs_material"
      action: "serve"
      port: 8000
```

