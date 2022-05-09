VERSION=0.1
IMAGE_NAME=microci_ipp

all:
#all: publish

build:
	docker build -t intmain/$(IMAGE_NAME):0.1 -f Dockerfile .

test: build
	echo "Fixme"

publish: build
	echo "Fixme"

.PHONY: all build test publish
