VERSION=0.5.1
IMAGE_NAME=microci_clang

all: publish

build:
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile --network=host .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it intmain/$(IMAGE_NAME):$(VERSION)
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) clang-tidy-html || echo OK

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
