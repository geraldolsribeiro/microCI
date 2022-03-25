[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin minio

```yaml
steps:
  - name: "Salvar artefato em bucket"
    plugin:
      name: minio
      bash: |
        mc ls microci/nome_do_bucket
        mc cp Makefile microci/nome_do_bucket/
        mc cp microci/nome_do_bucket/imagem.png .
```

