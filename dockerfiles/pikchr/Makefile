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

build:
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
