all: \
	microci_cppcheck \
	microci_mkdocs_material \
	microci_plantuml \
	microci_junit2html

dockerhub: \
	microci_cppcheck_push_dockerhub \
	microci_mkdocs_material_push_dockerhub \
	microci_plantuml_push_dockerhub \
	microci_junit2html_push_dockerhub

microci_ipp:
	docker build -t intmain/microci_ipp:0.1 -f ipp.Dockerfile .

microci_plantuml:
	docker build -t intmain/microci_plantuml:0.1 -f plantuml.Dockerfile .
	docker tag intmain/microci_plantuml:0.1 intmain/microci_plantuml:latest
	docker run -it intmain/microci_plantuml:0.1 plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/microci_plantuml:0.1 plantuml teste.pu

microci_mkdocs_material:
	docker build -t intmain/microci_mkdocs_material:0.1 -f mkdocs_material.Dockerfile .
	docker tag intmain/microci_mkdocs_material:0.1 intmain/microci_mkdocs_material:latest
	docker run -it intmain/microci_mkdocs_material:0.1 mkdocs --help
	docker run -it intmain/microci_mkdocs_material:0.1 python -c "import os; print(*os.environ['PATH'].split(os.pathsep), sep='\n')"

microci_cppcheck:
	docker build -t intmain/microci_cppcheck:0.3 -f cppcheck.Dockerfile .
	docker run -it intmain/microci_cppcheck:0.3 clang-tidy-html || echo OK
	docker run -it intmain/microci_cppcheck:0.3 cppcheck --help
	docker run -it intmain/microci_cppcheck:0.3

microci_junit2html:
	docker build -t intmain/microci_junit2html:0.1 -f junit2html.Dockerfile .
	docker tag intmain/microci_junit2html:0.1 intmain/microci_junit2html:latest
	docker run -it intmain/microci_junit2html:latest junit2html -h

microci_plantuml_push_dockerhub: microci_plantuml
	docker login
	docker push intmain/microci_plantuml:0.1
	docker tag intmain/microci_plantuml:0.1 intmain/microci_plantuml:latest
	docker push intmain/microci_plantuml:latest

microci_mkdocs_material_push_dockerhub: microci_mkdocs_material
	docker login
	docker push intmain/microci_mkdocs_material:0.1
	docker tag intmain/microci_mkdocs_material:0.1 intmain/microci_mkdocs_material:latest
	docker push intmain/microci_mkdocs_material:latest

microci_cppcheck_push_dockerhub: microci_cppcheck
	docker login
	docker push intmain/microci_cppcheck:0.3
	docker tag intmain/microci_cppcheck:0.3 intmain/microci_cppcheck:latest
	docker push intmain/microci_cppcheck:latest

microci_junit2html_push_dockerhub: microci_junit2html
	docker login
	docker push intmain/microci_junit2html:0.1
	docker tag intmain/microci_junit2html:0.1 intmain/microci_junit2html:latest
	docker push intmain/microci_junit2html:latest
