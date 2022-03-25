[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin clang-tidy

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

