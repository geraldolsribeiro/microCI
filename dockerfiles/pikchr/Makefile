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
IMAGE_NAME=microci_pikchr

all: publish

update:
	rm -rf pikchr_src
	git clone https://github.com/geraldolsribeiro/pikchr.git pikchr_src
	sed -i "s/^\(CFLAGS =.*\)/\1 -static/" pikchr_src/Makefile
	make -C pikchr_src
	cp pikchr_src/pikchr .
	rm -rf pikchr_src
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/:/microci_workspace \
		intmain/$(IMAGE_NAME):latest \
		pikchr --svg-only sample.pikchr > sample.svg
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/:/microci_workspace \
		intmain/$(IMAGE_NAME):latest \
	 	inkscape \
                 --export-type=pdf \
                 --export-overwrite \
                 --export-filename sample1.pdf sample.svg
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/:/microci_workspace \
		intmain/$(IMAGE_NAME):latest \
		rsvg-convert --format=pdf sample.svg > sample2.pdf


publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
