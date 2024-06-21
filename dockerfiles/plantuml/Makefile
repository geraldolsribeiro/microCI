VERSION=0.5
IMAGE_NAME=microci_plantuml

all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/$(IMAGE_NAME):$(VERSION) plantuml teste.pu

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
