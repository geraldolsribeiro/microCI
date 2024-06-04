[//]: <> (Documentação gerada com intmain_docmd)
# Usage example for the clang-tidy plugin


```yaml
steps:
  - name: "Gerar relatório de verificação do código C++ - clang-tidy"
    description: "Verifica o código C++ e gera relatório em formato HTML"
    plugin:
      name: "clang-tidy"
      options:
        - "-std=c++11"
      include:
        - "/usr/include/"
      source:
        - "src/*.cpp"
        - "test/*.cpp"
```

