# MIT License
#
# Copyright (c) 2022-2025 Geraldo Luis da Silva Ribeiro
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

VERSION=0.2
IMAGE_NAME=microci_ghdl

all: publish

.PHONY: update
update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

.PHONY: build
build:
	DOCKER_BUILDKIT=0 docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile --network=host .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

.PHONY: bash
bash:
	docker run --rm \
		--volume $(shell pwd):/microci_workspace \
		-it intmain/$(IMAGE_NAME):$(VERSION) \
		bash -l

.PHONY: test
test:
	docker run --rm -it intmain/$(IMAGE_NAME):$(VERSION)
	docker run --rm \
		--user $(shell id -u):$(shell id -g) \
		--volume $(shell pwd):/microci_workspace \
		-it intmain/$(IMAGE_NAME):$(VERSION) \
		ghdl -a --std=08 all08.vhdl

.PHONY: publish
publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: download_test
download_test: all08.vhdl ams08.vhdl

all08.vhdl:
	wget https://raw.githubusercontent.com/ghdl/ghdl/refs/heads/master/testsuite/sanity/004all08/all08.vhdl

ams08.vhdl:
	wget https://raw.githubusercontent.com/ghdl/ghdl/refs/heads/master/testsuite/sanity/004all08/ams08.vhdl

