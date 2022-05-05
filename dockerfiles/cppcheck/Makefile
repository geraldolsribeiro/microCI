VERSION=0.4.0

all: publish

build:
	docker build -t intmain/microci_cppcheck:$(VERSION) -f Dockerfile .
	docker tag intmain/microci_cppcheck:$(VERSION) intmain/microci_cppcheck:latest

test: build
	docker run -it intmain/microci_cppcheck:$(VERSION) clang-tidy-html || echo OK
	docker run -it intmain/microci_cppcheck:$(VERSION) cppcheck --help
	docker run -it intmain/microci_cppcheck:$(VERSION)

publish: build
	docker login
	docker push intmain/microci_cppcheck:$(VERSION)
	docker push intmain/microci_cppcheck:latest

.PHONY: all build test publish
