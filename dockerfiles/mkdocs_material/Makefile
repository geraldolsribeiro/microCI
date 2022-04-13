VERSION=0.3

all: publish

build:
	docker build -t intmain/microci_mkdocs_material:$(VERSION) -f Dockerfile .
	docker tag intmain/microci_mkdocs_material:$(VERSION) intmain/microci_mkdocs_material:latest

test: build
	docker run -it intmain/microci_mkdocs_material:$(VERSION) mkdocs --help
	docker run -it intmain/microci_mkdocs_material:$(VERSION) python -c "import os; print(*os.environ['PATH'].split(os.pathsep), sep='\n')"

publish: build
	docker login
	docker push intmain/microci_mkdocs_material:$(VERSION)
	docker push intmain/microci_mkdocs_material:latest

.PHONY: all build test publish
