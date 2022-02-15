# Bem-vindo ao microCI

A documentação completa pode ser encontrada em [microCI.dev](https://microci.dev).

## Requisitos

* Ambiente docker corretamente configurado

## Instalação

```bash
curl -L https://github.com/geraldolsribeiro/microCI/releases/download/0.5.0/microCI \
  -o /usr/local/bin/microCI
chmod 755 /usr/local/bin/microCI
```

## Por fazer

* criar pacote deb
* criar pacote rpm
* passar usuário atual para container para evitar geração de arquivos como `root` na pasta de trabalho
* incluir mensagem de como usar no final do script
* abortar caso algum passo falhar
* exibir mensagem de erro do passo
* substituição de variáveis nos steps
* uso de variáveis de ambiente e segredos configurados no servidor

## Versões

* `0.5.0` Montagem de volumes; Cópia de chave SSH
* `0.4.0` Construção do microCI pelo microCI
* `0.3.0` Cria arquivos de configuração
* `0.2.0` Plugin mkdocs
* `0.1.0` Passo em bash e plugin git_deploy
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
 
