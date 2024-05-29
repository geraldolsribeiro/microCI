VERSION=0.1
IMAGE_NAME=microci_ipp

all:
#all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):0.1 -f Dockerfile .

test:
	echo "Fixme"

publish: build
	echo "Fixme"

.PHONY: all build test publish
