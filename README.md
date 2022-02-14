# microCI

## Requisitos

* Ambiente docker corretamente configurado

## Instalação

```bash
curl -L https://github.com/geraldolsribeiro/microCI/releases/download/0.4.0/microCI \
  -o /usr/local/bin/microCI
chmod 755 /usr/local/bin/microCI
```

## Por fazer

* passar usuário atual para container para evitar geração de arquivos como `root` na pasta de trabalho
* incluir mensagem de como usar no final do script
* abortar caso algum passo falhar
* exibir mensagem de erro do passo
* substituição de variáveis nos steps
* uso de variáveis de ambiente e segredos configurados no servidor

## Versões

* `0.4.0` Construção do microCI pelo microCI
* `0.3.0` Cria arquivos de configuração
* `0.2.0` Plugin mkdocs
* `0.1.0` Passo em bash e plugin git_deploy
* `0.0.1` Versão inicial para prova de conceito

