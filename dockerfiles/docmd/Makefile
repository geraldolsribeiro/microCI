VERSION=0.1
IMAGE_NAME=microci_docmd

all: publish

build:
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
