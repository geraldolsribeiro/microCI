VERSION=0.4
IMAGE_NAME=microci_mkdocs_material

all: publish

update:
	docker pull $(shell awk '/^FROM/ { print $$2; exit }' Dockerfile)

build: update
	docker build -t intmain/$(IMAGE_NAME):$(VERSION) -f Dockerfile .
	docker tag intmain/$(IMAGE_NAME):$(VERSION) intmain/$(IMAGE_NAME):latest

test:
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) mkdocs --help
	docker run -it intmain/$(IMAGE_NAME):$(VERSION) python -c "import os; print(*os.environ['PATH'].split(os.pathsep), sep='\n')"

publish: build
	docker login
	docker push intmain/$(IMAGE_NAME):$(VERSION)
	docker push intmain/$(IMAGE_NAME):latest

.PHONY: all build test publish
