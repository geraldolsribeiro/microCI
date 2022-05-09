VERSION=0.4.0
IMAGE_NAME=microci_cppcheck

all: publish

build:
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test: build
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) cppcheck --help
	docker run -it intmain/$(IMAGE_NAME):$(VERSION)

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
