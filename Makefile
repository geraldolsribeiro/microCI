SHELL=bash

all:

docs/plugin_%.md: new/%.yml
	INTMAIN_DOCMD_DETAILS=false INTMAIN_DOCMD_TOC=false INTMAIN_DOCMD_SHOW_SOURCE=false \
		intmain_docmd yaml $^ $@

doc_plugin: \
  docs/plugin_bash.md \
  docs/plugin_beamer.md \
  docs/plugin_clang-format_config.md \
  docs/plugin_clang-format.md \
  docs/plugin_clang-tidy.md \
  docs/plugin_cppcheck.md \
  docs/plugin_fetch2.md \
  docs/plugin_fetch.md \
  docs/plugin_git_deploy.md \
  docs/plugin_git_publish.md \
  docs/plugin_minio.md \
  docs/plugin_mkdocs_material_config.md \
  docs/plugin_mkdocs_material.md \
  docs/plugin_npm.md \
  docs/plugin_plantuml.md \
  docs/plugin_skip.md \
