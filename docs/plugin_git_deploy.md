[//]: <> (Documentation generated by intmain_docmd)
# git_deploy plugin


```yaml
---
steps:
  - name: "Deploy de página de aplicação VueJS"
    description: "Descrição do passo"
    plugin:
      name: "git_deploy"
      # Repositório contendo a última versão da aplicação
      repo: "git@gitlab.xyx.com.br:str/app_frontend_deploy.git"
      # Local no servidor onde ficará o repositório git local
      git_dir: "/opt/microCI/repos/app_frontend_deploy"
      # Local no servidor onde será feito o deploy dos arquivos
      work_tree: "/var/www/my-intranet/html/app"
      clean: true
```

