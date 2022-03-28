[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin git_publish

```yaml
---
steps:
  - name: "Publicar em outro repositório git"
    description: "Descrição deste passo"
    # network: bridge # (default)
    # run_as: root
    # run_as: user # (default)
    ssh:
      copy_from: "${HOME}/.ssh"
      # copy_to: "/root/.ssh"          # root
      # copy_to: "/tmp/microci_deploy" # user (default)
    plugin:
      name: "git_publish"
      git_url: "git@seuservidor.com.br:awesome_deploy.git"
      copy_to: "/tmp/deploy" # Pasta padrão dentro do container
      copy_from: "site"      # Pasta padrão que contêm os arquivos
      clean_before: true     # Remove todos os arquivos antes de copiar o novos
```
