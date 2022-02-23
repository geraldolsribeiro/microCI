all: \
	microci_cppcheck_test

microci_cppcheck_image:
	docker build -t intmain/microci_cppcheck:0.2 -f cppcheck.Dockerfile .

microci_cppcheck_test: microci_cppcheck_image
	docker run -it intmain/microci_cppcheck:0.2 cppcheck --help
	docker run -it intmain/microci_cppcheck:0.2

microci_cppcheck_push_dockerhub: microci_cppcheck_test
	docker login
	docker push intmain/microci_cppcheck:0.2
	docker tag intmain/microci_cppcheck:0.2 intmain/microci_cppcheck:latest
	docker push intmain/microci_cppcheck:latest
