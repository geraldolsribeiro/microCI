# MIT License
#
# Copyright (c) 2022-2024 Geraldo Luis da Silva Ribeiro
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

VERSION=0.1
IMAGE_NAME=microci_omnetpp

ifneq (,$(wildcard ./.env))
	include .env
	export
endif

.PHONY: all
all: build publish

.PHONY: update
update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

.PHONY: build
build: update
	docker build --progress=plain --tag intmain/$(IMAGE_NAME):$(VERSION) --file Dockerfile .
	docker tag                          intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

.PHONY: test
test:
	docker run -it --rm intmain/$(IMAGE_NAME):latest | grep -q "OMNET++" 

.PHONY: bash
bash:
	docker run -it \
		--rm \
		--network host \
		--env DISPLAY=$(DISPLAY) \
		--volume $(shell pwd):/work \
		--volume /tmp/.X11-unix:/tmp/.X11-unix \
		intmain/$(IMAGE_NAME):latest bash -l

.PHONY: publish
publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
