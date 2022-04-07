VERSION=0.2

all: publish

build:
	docker build -t intmain/microci_plantuml:0.2 -f Dockerfile .
	docker tag intmain/microci_plantuml:0.2 intmain/microci_plantuml:latest

test: build
	docker run -it intmain/microci_plantuml:0.2 plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/microci_plantuml:0.2 plantuml teste.pu

publish: build
	docker login
	docker push intmain/microci_plantuml:0.2
	docker push intmain/microci_plantuml:latest

.PHONY: all build test publish
