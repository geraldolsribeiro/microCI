all: \
	microci_cppcheck_push_dockerhub \
	microci_mkdocs_material_push_dockerhub \
	microci_plantuml_push_dockerhub \
	microci_junit2html_push_dockerhub \
	microci_poppler_push_dockerhub \
	microci_docmd_push_dockerhub

microci_ipp:
	docker build -t intmain/microci_ipp:0.1 -f ipp.Dockerfile .

microci_plantuml:
	docker build -t intmain/microci_plantuml:0.2 -f plantuml.Dockerfile .
	docker tag intmain/microci_plantuml:0.2 intmain/microci_plantuml:latest
	docker run -it intmain/microci_plantuml:0.2 plantuml -h
	docker run -it --user $(shell id -u):$(shell id -g) -v${PWD}:/ws --workdir /ws intmain/microci_plantuml:0.2 plantuml teste.pu

microci_plantuml_push_dockerhub: microci_plantuml
	docker login
	docker push intmain/microci_plantuml:0.2
	docker push intmain/microci_plantuml:latest

microci_mkdocs_material:
	docker build -t intmain/microci_mkdocs_material:0.2 -f mkdocs_material.Dockerfile .
	docker tag intmain/microci_mkdocs_material:0.2 intmain/microci_mkdocs_material:latest
	docker run -it intmain/microci_mkdocs_material:0.2 mkdocs --help
	docker run -it intmain/microci_mkdocs_material:0.2 python -c "import os; print(*os.environ['PATH'].split(os.pathsep), sep='\n')"

microci_mkdocs_material_push_dockerhub: microci_mkdocs_material
	docker login
	docker push intmain/microci_mkdocs_material:0.2
	docker push intmain/microci_mkdocs_material:latest

microci_cppcheck:
	docker build -t intmain/microci_cppcheck:0.3 -f cppcheck.Dockerfile .
	docker run -it intmain/microci_cppcheck:0.3 clang-tidy-html || echo OK
	docker run -it intmain/microci_cppcheck:0.3 cppcheck --help
	docker run -it intmain/microci_cppcheck:0.3

microci_cppcheck_push_dockerhub: microci_cppcheck
	docker login
	docker push intmain/microci_cppcheck:0.3
	docker push intmain/microci_cppcheck:latest

microci_junit2html:
	docker build -t intmain/microci_junit2html:0.1 -f junit2html.Dockerfile .
	docker tag intmain/microci_junit2html:0.1 intmain/microci_junit2html:latest
	docker run -it intmain/microci_junit2html:latest junit2html -h

microci_junit2html_push_dockerhub: microci_junit2html
	docker login
	docker push intmain/microci_junit2html:0.1
	docker push intmain/microci_junit2html:latest

microci_poppler:
	docker build -t intmain/microci_poppler:0.1 -f poppler.Dockerfile .
	docker tag intmain/microci_poppler:0.1 intmain/microci_poppler:latest
	docker run -it intmain/microci_poppler:latest pdftoppm -h

microci_poppler_push_dockerhub: microci_poppler
	docker login
	docker push intmain/microci_poppler:0.1
	docker push intmain/microci_poppler:latest

microci_docmd:
	docker build -t intmain/microci_docmd:0.1 -f docmd.Dockerfile .
	docker tag intmain/microci_docmd:0.1 intmain/microci_docmd:latest
	docker run -it --rm --user $(shell id -u):$(shell id -g) \
		-v ${PWD}/../:/microci_workspace intmain/microci_docmd:latest \
		ruby dockerfiles/intmain_docmd docs/intmain_docmd.md

microci_docmd_push_dockerhub: microci_docmd
	docker login
	docker push intmain/microci_docmd:0.1
	docker push intmain/microci_docmd:latest

