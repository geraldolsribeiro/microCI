VERSION=0.1

all: publish

build:
	docker build -t intmain/microci_junit2html:$(VERSION) -f Dockerfile .
	docker tag intmain/microci_junit2html:$(VERSION) intmain/microci_junit2html:latest

test: build
	docker run -it intmain/microci_junit2html:latest junit2html -h

publish: build
	docker login
	docker push intmain/microci_junit2html:$(VERSION)
	docker push intmain/microci_junit2html:latest

.PHONY: all build test publish
