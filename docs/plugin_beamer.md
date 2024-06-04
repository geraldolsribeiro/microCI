[//]: <> (Documentação gerada com intmain_docmd)
# Usage example for the beamer plugin


```yaml
steps:
  - name: Criar apresentação PDF a partir de arquivos Markdown
    plugin:
      name: beamer
      output: nome_da_minha_apresentacao.pdf
      source: # Lista de arquivos markdown na sequência em que serão utilizados
        - 00-intro.md
        - 01-outro.md
        - 02-mais-um.md
        - 99-referencias.md
      lang: pt-BR # en-US
      institute: Nome da instituição
      date: 01 de Abril de 2023
      title: Título da apresentação
      subtitle: Subtítulo da apresentação
      author:
        - Geraldo Ribeiro
        - Outro Nome de Autor
      # subject: Informação da propriedade Assunto do PDF
      # keywords: [palavra-chave1, palavra-chave2, palavra-chave3, PDF]
      # slide-level: 2   # usa o título de segundo nível para iniciar um slide
      # aspectratio: 169 # Formato 16:9
      # links-as-notes: true
      # natbib: true
      # bibliography: refs.bib
      # theme: STR
      header-includes:
        - \usepackage{tikz}
```

