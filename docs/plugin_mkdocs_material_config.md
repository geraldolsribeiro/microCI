[//]: <> (Documentação gerada com intmain_docmd)
# Usage example for the mkdocs_material_config plugin


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
```

