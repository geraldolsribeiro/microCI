VERSION=0.2
IMAGE_NAME=microci_pandoc

all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	DOCKER_BUILDKIT=0 docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test: build
	docker run -it intmain/$(IMAGE_NAME):latest --help

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
