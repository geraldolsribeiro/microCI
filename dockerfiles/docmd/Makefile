VERSION=0.3
IMAGE_NAME=microci_docmd

all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/../../:/microci_workspace \
		intmain/$(IMAGE_NAME):latest \
		ruby dockerfiles/docmd/intmain_docmd docs/intmain_docmd.md

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
