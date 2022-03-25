[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin fetch

```yaml
---
steps:
  - name: "Baixar arquivos externos ao projeto"
    description: "Download de dependências utilizadas na compilação"
    ssh: # usado pelo git_archive
      copy_from: "${HOME}/.ssh"
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
        - git_archive: git@gitlabcorp.xyz.com.br:group/repo.git
          target: /tmp/lib/
          strip-components: 2 # remove 2 níveis 
          files:
            - path1/path2/lib/*.so
        - url: https://raw.githubusercontent.com/adishavit/argh/master/argh.h
          target: /tmp/include # Local onde será colocado este arquivo
        - url: https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
        - url: https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp
```

