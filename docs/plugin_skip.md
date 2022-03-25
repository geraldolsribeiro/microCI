[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin skip

```yaml
steps:
  - name: "Passo ignorado"
    description: "Este passo está desativado"
    plugin:
      name: skip
  - name: "Compilar versão estática do microCI"
    docker: "gcc:11"
    network: bridge
    run_as: root
    plugin:
      name: skip # Este passo não será executado
      bash: |
        apt update
        apt upgrade -y
        apt install -y xxd libspdlog-dev libyaml-cpp-dev
        make -C src clean all
        make -C test
        # Ajusta o dono do arquivo do root para o usuário atual
        chown $(id -u).$(id -g) -Rv src/microCI
```

