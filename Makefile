### ----------------------------------------------------------------------
### Copyright (C) 2022-2026 Geraldo Ribeiro <geraldo@intmain.io>
### ----------------------------------------------------------------------

SHELL=bash

VERSION=$(shell sed -n "s/#define microCI_version \"\([0-9\.]\+\)\"/\1/p" include/MicroCI.hpp)
FIRST_GIT_COMMIT=$(shell git rev-list --max-parents=0 HEAD)

.PHONY: all
all:
# all: doc_plugin

# install-docmd:
# 	cargo install docmd
#
# docs/plugin_%.md: new/%.yml
# 	@echo "Writing $@..."
# 	@DOCMD_DETAILS=false DOCMD_TOC=false DOCMD_SHOW_SOURCE=false \
# 		docmd yaml $^ $@
#
# .PHONY: doc_plugin
# doc_plugin: \
#   docs/plugin_bash.md \
#   docs/plugin_beamer.md \
#   docs/plugin_clang-format_config.md \
#   docs/plugin_clang-format.md \
#   docs/plugin_clang-tidy.md \
#   docs/plugin_cppcheck.md \
#   docs/plugin_fetch2.md \
#   docs/plugin_fetch.md \
#   docs/plugin_git_deploy.md \
#   docs/plugin_git_publish.md \
#   docs/plugin_minio.md \
#   docs/plugin_jfrog.md \
#   docs/plugin_mkdocs_material_config.md \
#   docs/plugin_mkdocs_material.md \
#   docs/plugin_npm.md \
#   docs/plugin_doxygen.md \
#   docs/plugin_pandoc.md \
#   docs/plugin_pikchr.md \
#   docs/plugin_plantuml.md \
#   docs/plugin_skip.md

.PHONY: build
build:
	microCI | bash

.PHONY: test
test:
	$(MAKE) -C test

microci.equivs:
	equivs-control $@

.PHONY: deb
deb: microci.equivs
	./git2dch.sh > debian/changelog
	sed -i "s/^Version:.*/Version: $(VERSION)/" $<
	DEB_BUILD_OPTIONS=nostrip equivs-build --full $<

.PHONY: rpm
rpm: deb
	alien -r microci_$(VERSION)_amd64.deb

.PHONY: clean
clean:
	$(RM) microci_*_amd64.buildinfo
	$(RM) microci_*_amd64.changes
	$(RM) microci_*_amd64.deb
	$(RM) microci_*.dsc
	$(RM) microci_*.tar.xz
	$(RM) microci-*.x86_64.rpm
	$(RM) microci_*_amd64.buildinfo
	$(RM) microci_*_amd64.changes
	$(RM) microci_*_amd64.deb
	$(RM) microci_*.dsc
	$(RM) microci_*.tar.xz
