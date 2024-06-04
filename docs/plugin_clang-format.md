[//]: <> (Documentação gerada com intmain_docmd)
# Usage example for the clang-format plugin


```yaml
steps:
  - name: "Formatar código C++ com clang-format"
    description: "Padroniza formato do código usando regras"
    run_as: user
    plugin:
      name: "clang-format"
      source: # Lista arquivos a serem formatados
        - "src/*.cpp"
        - "test/*.cpp"
        - "include/*.hpp"
```

