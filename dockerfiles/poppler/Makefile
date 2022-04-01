VERSION=0.1

all: publish

build:
	docker build -t intmain/microci_poppler:$(VERSION) -f poppler.Dockerfile .
	docker tag intmain/microci_poppler:$(VERSION) intmain/microci_poppler:latest

test: build
	docker run -it intmain/microci_poppler:latest pdftoppm -h

publish: build
	docker login
	docker push intmain/microci_poppler:$(VERSION)
	docker push intmain/microci_poppler:latest

.PHONY: all build test publish
