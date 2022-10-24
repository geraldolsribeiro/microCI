[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin minio

Veja documentação do **MinIO Client** em <https://github.com/minio/mc>.


```yaml
steps:
  - name: "Salvar artefato em bucket"
    description: "Descrição deste passo"
    run_as: user
    envs:
      HOME: /tmp/
    plugin:
      name: minio
      bash: |
        # Lista arquivos do bucket
        mc ls microci/nome_do_bucket
        # Copia arquivos do workspace para o bucket
        mc cp Makefile microci/nome_do_bucket/
        # Copia arquivos do bucket para o workspace
        mc cp microci/nome_do_bucket/imagem.png .
```

