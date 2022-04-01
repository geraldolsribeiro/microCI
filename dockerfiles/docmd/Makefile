VERSION=0.1

all: publish

build:
	docker build -t intmain/microci_docmd:$(VERSION) -f docmd.Dockerfile .
	docker tag intmain/microci_docmd:$(VERSION) intmain/microci_docmd:latest

test:
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/../../:/microci_workspace \
		intmain/microci_docmd:latest \
		ruby dockerfiles/docmd/intmain_docmd docs/intmain_docmd.md

publish: build
	docker login
	docker push intmain/microci_docmd:$(VERSION)
	docker push intmain/microci_docmd:latest

.PHONY: all build test publish
