VERSION=0.3
IMAGE_NAME=microci_plantuml

all: publish

build:
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test: build
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/$(IMAGE_NAME):$(VERSION) plantuml teste.pu

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
