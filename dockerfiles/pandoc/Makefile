VERSION=0.2
IMAGE_NAME=microci_pandoc

.PHONY: all
all: publish

.PHONY: update
update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

.PHONY: build
build: update
	DOCKER_BUILDKIT=0 docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

.PHONY: test
test:
	docker run -it intmain/$(IMAGE_NAME):latest --print-default-template=latex > default.latex
	docker run -it intmain/$(IMAGE_NAME):latest --help

.PHONY: publish
publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: bash
bash:
	docker run --rm -it --entrypoint /bin/sh intmain/microci_pandoc:latest

.PHONY: fc-list
fc-list:
	docker run --rm -it --entrypoint fc-list intmain/microci_pandoc:latest
