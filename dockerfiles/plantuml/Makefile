VERSION=0.3

all: publish

build:
	docker build -t intmain/microci_plantuml:$(VERSION) -f Dockerfile .
	docker tag intmain/microci_plantuml:$(VERSION) intmain/microci_plantuml:latest

test: build
	docker run -it intmain/microci_plantuml:$(VERSION) plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/microci_plantuml:$(VERSION) plantuml teste.pu

publish: build
	docker login
	docker push intmain/microci_plantuml:$(VERSION)
	docker push intmain/microci_plantuml:latest

.PHONY: all build test publish
