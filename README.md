# Bem-vindo ao microCI

A documentação completa pode ser encontrada em [microci.dev](https://microci.dev).

O `microCI` é uma ferramenta para auxiliar a construção de pipelines de
integração contínua.

O principal diferencial do `microCI` é a geração de **scripts portáveis** e
totalmente **auditáveis** para execução de tarefas em **containers docker**.

Como todos os passos são executados em um ou mais containers docker, todo
o processo se torna reprodutível em: a) qualquer máquina do time de
desenvolvimento, b) no servidor que realizará o build automático após os
commits, c) no servidor que realizará o deploy para produção... Enfim a mesma
ferramenta, sem dependência, pode ser usada tanto na máquina do dev quanto no
servidor.

## No desktop

A utilização do `microCI` no desktop do dev é muito simples:

```bash
microCI | bash
```

No comando acima o `microCI` leu um arquivo `.microCI.yml` contendo a descrição
dos passos da pipeline e gerou um script para execução destes passos. Este
script é escrito na saída padrão e pode ser encadeado com outras ferramentas ou
passado para um interpretador `bash` para a execução.

Um arquivo chamado `.microCI.log` é gerado com todas as mensagens, warning,
erros de todos os comandos executados e pode ser analisado para resolver algum
problema que possa ocorrer.

## No servidor

Para execução no servidor basta expor **webhooks** que, ao serem acionados,
executam `microCI` nas respectivas pastas do projeto.

## Requisitos mínimos

* Um ambiente docker corretamente configurado
* Bash

## Instalação

A instalação é igualmente simples, bastando copiar o executável para uma pasta
que esteja no `PATH`. Para remover o `microCI` completamente do sistema basta
apagar o arquivo copiado para o PATH.

```bash
curl -fsSL https://github.com/geraldolsribeiro/microci/releases/latest/download/microCI \
  -o /usr/local/bin/microCI
chmod 755 /usr/local/bin/microCI
```

## Container docker

Você pode utilizar **qualquer container docker** para execução, tanto os
públicos do [Docker HUB](https://hub.docker.com/) quanto os que você
desenvolver internamente.

## Integração com github, gitlab, ...

O modo mais simples e seguro de utilizar o `microCI` para ler ou escrever nos
repositórios é realizar o acesso através de chave SSH.

## Por fazer

Abaixo estão algumas funcionalidades que poderão estar presente nos próximos
releases.

* criar pacote deb
* criar pacote rpm
* passar usuário atual para container para evitar geração de arquivos como
  `root` na pasta de trabalho
* exibir mensagem de erro do passo que falhou
* substituição de variáveis nos steps
* uso de variáveis de ambiente e segredos configurados no servidor
* criar imagem docker a partir de repositório git

## Versões

* `0.25.5` Plugin `git_publish` com escolha de branch (p.e. gh-pages)
* `0.24.0` Plugin `docker_build`
* `0.23.0` Plugin `fetch` download github
* `0.22.0` Plugin `fetch` download de pastas no github
* `0.21.0` Plugin `flawfinder`
* `0.20.0` Imagem `intmain/microci_clang`
* `0.19.0` Documentação com `intmain_docmd`
* `0.18.0` Plugin `minio`
* `0.17.0` Plugin `skip`; Status de execução
* `0.16.1` Refatoração dos plugins; Execução default como não root
* `0.15.2` Plugin `fetch` URL e pastas do gitlab
* `0.14.1` Plugin `beamer`
* `0.13.0` Configuração de rede docker
* `0.12.0` Plugins `clang-format` e `junit2html`
* `0.11.1` Notificação em canal do discord
* `0.10.1` Múltiplos arquivos de modelo
* `0.9.0` Plugin `plantuml`
* `0.8.0` Plugin `clang-tidy`
* `0.7.0` Plugin `cppcheck`
* `0.6.1` Plugin `git_publish`
* `0.5.0` Montagem de volumes; Cópia de chave SSH
* `0.4.0` Construção do microCI pelo microCI
* `0.3.0` Cria arquivos de configuração
* `0.2.0` Plugin `mkdocs`
* `0.1.0` Passo em `bash` e plugin `git_deploy`
* `0.0.1` Versão inicial para prova de conceito

## Licença

MIT License

Copyright (c) 2022 Geraldo Luis da Silva Ribeiro

```
░░░░░░░░░░░░░░░░░
░░░░░░░█▀▀░▀█▀░░░
░░░█░█░█░░░░█░░░░
░░░█▀▀░▀▀▀░▀▀▀░░░
░░░▀░░░░░░░░░░░░░
░░░░░░░░░░░░░░░░░
```

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
