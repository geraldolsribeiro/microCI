[//]: <> (Documentação gerada com intmain_docmd)
# Exemplo de configuração do plugin mkdocs_material

```yaml
site_name: Nome do site
repo_url: https://github.com/geraldolsribeiro/microCI
repo_name: geraldolsribeiro/microCI
edit_uri: edit/master/docs/
theme:
  name: material
  language: pt
  logo: assets/logo.svg
  favicon: assets/favicon.png
  palette:
    primary: white
  icon:
    repo: fontawesome/brands/github
features:
  - navigation.instant
extra:
  homepage: http://url-final-do-projeto/
extra_css:
  - css/print.css
markdown_extensions:
  - admonition
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
  - i18n:
      default_language: pt
      languages:
        en:
          name: English
          build: true
          site_name: microCI documentation
        pt:
          name: Português
          build: true
          site_name: Documentação do microCI
```
