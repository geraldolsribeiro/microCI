[//]: <> (Documentação gerada com intmain_docmd)

```yaml
site_name: Site name
repo_url: https://github.com/geraldolsribeiro/microCI
repo_name: geraldolsribeiro/microCI
edit_uri: edit/master/docs/
theme:
  name: material
  language: pt
  logo: images/logo.svg
  favicon: images/favicon.png
  palette:
    primary: white
  icon:
    repo: fontawesome/brands/github
    # repo: fontawesome/brands/gitlab
features:
  - navigation.instant
extra:
  homepage: http://url-final-do-projeto/
extra_css:
  - css/print.css
markdown_extensions:
  - admonition
  - def_list
  # Para utilizar atributos nas imagens
  # ![WebCapture](/img/support/prod/screenshot-web-capture.png){: style="height:150px;width:150px"}
  - attr_list
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_code_format
  - pymdownx.tasklist:
      custom_checkbox: true
  - pymdownx.emoji:
      emoji_index: !!python/name:materialx.emoji.twemoji
      emoji_generator: !!python/name:materialx.emoji.to_svg
  - pymdownx.highlight:
      anchor_linenums: true
  - pymdownx.inlinehilite
  - pymdownx.mark
  - pymdownx.snippets
plugins:
  - search:
      lang: pt
  - mermaid2
# - i18n:
  default_language: pt
#     default_language: pt
  languages:
#     languages:
    en:
#       en:
      name: English
#         name: English
      build: true
#         build: true
      site_name: microCI documentation
#         site_name: microCI documentation
    pt:
#       pt:
      name: Português
#         name: Português
      build: true
#         build: true
      site_name: Documentação do microCI
#         site_name: Documentação do microCI
```

