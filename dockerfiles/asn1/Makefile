# MIT License
#
# Copyright (c) 2022-2026 Geraldo Luis da Silva Ribeiro
#
# ░░░░░░░░░░░░░░░░░
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░
# ░░░█▀▀░▀▀▀░▀▀▀░░░
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

VERSION=0.2.0
IMAGE_NAME=microci_asn1
PLATFORMS=linux/amd64,linux/arm64

.PHONY: all
all: publish

.PHONY: update
update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

.PHONY: build
build: update
	docker buildx build --platform $(PLATFORMS) --load -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

.PHONY: test
test:
	rm -rf test
	docker run --rm intmain/$(IMAGE_NAME):latest \
		/opt/asn1scc/asn1scc --help || echo "OK --help exits with error :("
	docker run --rm \
		--user $(shell id -u):$(shell id -g) \
		--volume ${PWD}:/microci_workspace \
		intmain/$(IMAGE_NAME):latest \
		/opt/asn1scc/asn1scc --c-lang --uper-enc --auto-test-cases --out test sample.asn
	$(MAKE) -C test clean all coverage

.PHONY: publish
publish:: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

include ../UpdateReadme.mk
