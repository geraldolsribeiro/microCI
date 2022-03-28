[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de uso do plugin bash

```yaml
steps:
  - name: "Compilar versão estática do microCI"
    description: "Descrição deste passo"
    docker: "gcc:11" # Imagem que será executada
    network: bridge  # bridge, host ou none
    run_as: root     # root, user
    plugin:
      name: bash
      bash: |
        # Comentários são permitidos entre os comando
        apt update
        apt upgrade -y
        # Estes pacotes são instalados na imagem
        apt install -y xxd libspdlog-dev libyaml-cpp-dev
        make -C src clean all
        make -C test
        # Torna o usuário atual como dono do arquivo gerado
        chown $(id -u).$(id -g) -Rv src/microCI
```
