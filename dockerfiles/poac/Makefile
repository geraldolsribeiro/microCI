VERSION=0.1
IMAGE_NAME=microci_poac

all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it intmain/$(IMAGE_NAME):latest poac -h
	docker run -it intmain/$(IMAGE_NAME):latest pwd
	docker run -it --volume ${PWD}:/microci_workspace intmain/$(IMAGE_NAME):latest rm -rf myproj
	docker run -it --volume ${PWD}:/microci_workspace intmain/$(IMAGE_NAME):latest poac new myproj
	docker run -it --volume ${PWD}:/microci_workspace intmain/$(IMAGE_NAME):latest { cd myproj; poac run; }

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
