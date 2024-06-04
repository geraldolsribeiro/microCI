[//]: <> (Documentação gerada com intmain_docmd)
# Usage example for the cppcheck plugin


```yaml
steps:
  - name: "Gerar relatório de verificação do código C++ - cppcheck"
    description: "Verifica o código C++ e gera relatório em formato HTML"
    plugin:
      name: "cppcheck"
      options:
        # - "-j 2"             # executa 2 jobs simultâneamente
        # - "--clang=clang-11" # utiliza o clang como analisador
        # - "--bug-hunting"    # útil para code review
      include:
        - "include"
      source:
        - "src"
        - "test"
      platform: "unix64"
```

