mkdocs_material:
	docker build -t intmain/microci_mkdocs_material:0.1 -f mkdocs_material.Dockerfile .
	docker tag intmain/microci_mkdocs_material:0.1 intmain/microci_mkdocs_material:latest
	docker run -it intmain/microci_mkdocs_material:0.1 mkdocs --help

microci_ipp:
	docker build -t intmain/microci_ipp:0.1 -f ipp.Dockerfile .

all: \
	microci_ipp \
	microci_cppcheck_test

microci_cppcheck_image:
	docker build -t intmain/microci_cppcheck:0.3 -f cppcheck.Dockerfile .
	docker run -it intmain/microci_cppcheck:0.3 clang-tidy-html

microci_cppcheck_test: microci_cppcheck_image
	docker run -it intmain/microci_cppcheck:0.3 cppcheck --help
	docker run -it intmain/microci_cppcheck:0.3

microci_cppcheck_push_dockerhub: microci_cppcheck_test
	docker login
	docker push intmain/microci_cppcheck:0.3
	docker tag intmain/microci_cppcheck:0.3 intmain/microci_cppcheck:latest
	docker push intmain/microci_cppcheck:latest
