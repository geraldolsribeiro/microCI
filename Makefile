### ----------------------------------------------------------------------
### Copyright (C) 2022-2026 Geraldo Ribeiro <geraldolsribeiro@gmail.com>
### ----------------------------------------------------------------------

SHELL=bash

VERSION=$(shell sed -n "s/#define microCI_version \"\([0-9\.]\+\)\"/\1/p" include/MicroCI.hpp)
FIRST_GIT_COMMIT=$(shell git rev-list --max-parents=0 HEAD)

.PHONY: all
all: build

.PHONY: build
build:
	microCI -D | bash
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

.PHONY: publish
publish:
	bin/microCI --number 7,9,11 | bash
	# microCI -N 7 | bash
	# microCI -N 9 | bash
	# microCI -N 11 | bash
