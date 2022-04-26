VERSION=0.4

all: publish

build:
	docker build -t intmain/microci_clang:$(VERSION) -f Dockerfile .
	docker tag intmain/microci_clang:$(VERSION) intmain/microci_clang:latest

test:
	docker run -it intmain/microci_clang:$(VERSION)

publish: build
	docker login
	docker push intmain/microci_clang:$(VERSION)
	docker push intmain/microci_clang:latest

.PHONY: all build test publish
