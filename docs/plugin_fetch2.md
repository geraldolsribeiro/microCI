[//]: <> (Documentation generated by intmain_docmd)
# fetch plugin (2)


```yaml
---
steps:
  - name: "Baixar arquivos externos ao projeto"
    description: "Download de dependências utilizadas na compilação"
    ssh: # usado pelo git_archive
      copy_from: "${HOME}/.ssh"
      copy_to: "/home/bitnami/.ssh"
    plugin:
      name: "fetch"
      target: include # local padrão para os arquivos
      items:
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/include/
          files:
            - README.md
            - include/*.h
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/lib/
          files:
            - lib/*.so
        - url: https://raw.githubusercontent.com/adishavit/argh/master/argh.h
          target: /tmp/include # Local onde será colocado este arquivo
        - url: https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
        - url: https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp
```

