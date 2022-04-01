VERSION=0.2

all: publish

build:
	docker build -t intmain/microci_flawfinder:$(VERSION) -f flawfinder.Dockerfile .
	docker tag intmain/microci_flawfinder:$(VERSION) intmain/microci_flawfinder:latest

test: build
	docker run -it intmain/microci_flawfinder:latest --listrules

publish: build
	docker login
	docker push intmain/microci_flawfinder:$(VERSION)
	docker push intmain/microci_flawfinder:latest

.PHONY: all build test publish
