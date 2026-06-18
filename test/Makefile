# ░░░░░░░░░░░░░░░░░ microCI
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
# ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░ Copyright (c) 2022-2026 Geraldo Luis da Silva Ribeiro
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# Enable use of pushd in Makefile
SHELL=bash

%.test: % %.ref
	diff $(word 1,$^) $(word 2,$^) > /dev/null || echo "ERROR $(word 1,$^)"

test_old:
	rm -f .env
	rm -f .env.yml
	rm -f test_npm.yml
	for i in bash clang-tidy clang-format cppcheck git_deploy git_publish mkdocs_material npm plantuml template; do \
    echo "$$i" && rm -rf "new_$$i" && mkdir -p "new_$$i" \
		&& pushd "new_$$i" \
		&& pwd \
		&& ../../bin/microCI --new "$$i" \
		&& popd; \
	done
	cp dot_env_default .env
	cp dot_env_yml_default .env.yml
	../bin/microCI -i y.yml         || echo "OK"
	../bin/microCI --input=x.yml    || echo "OK"
	../bin/microCI -n invalid_type  || echo "OK"
	../bin/microCI                                               > default.sh
	../bin/microCI -h                                            > help.txt
	../bin/microCI -n npm -i test_npm.yml
	../bin/microCI -n npm -i test_npm.yml
	../bin/microCI -i ../snapshot_cmd_new/git_deploy.yml                      > git_deploy.sh
	../bin/microCI -i ../snapshot_cmd_new/mkdocs_material.yml                 > mkdocs_material.sh
	../bin/microCI -i ../snapshot_cmd_new/mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh
	../bin/microCI -i ../snapshot_cmd_new/doxygen.yml                         > doxygen.sh
	../bin/microCI -i ../snapshot_cmd_new/cppcheck.yml                        > cppcheck.sh
	../bin/microCI -i ../snapshot_cmd_new/vhdl-format.yml                     > vhdl-format.sh
	../bin/microCI -i ../snapshot_cmd_new/clang-tidy.yml                      > clang-tidy.sh
	../bin/microCI -i ../snapshot_cmd_new/clang-format.yml                    > clang-format.sh
	../bin/microCI -i ../snapshot_cmd_new/plantuml.yml                        > plantuml.sh
	../bin/microCI -i ../snapshot_cmd_new/pikchr.yml                          > pikchr.sh
	../bin/microCI -i ../snapshot_cmd_new/beamer.yml                          > beamer.sh
	../bin/microCI -i ../snapshot_cmd_new/mermaid.yml                         > mermaid.sh
	../bin/microCI -i ../snapshot_cmd_new/fetch.yml                           > fetch.sh
	../bin/microCI -i ../snapshot_cmd_new/skip.yml                            > skip.sh
	source skip.sh
	../bin/microCI -i ../snapshot_cmd_new/skip.yml  --append-log              > skip_append_log.sh
	source skip_append_log.sh
	cat .microCI.log > skip_append_log.log
	../bin/microCI -i ../snapshot_cmd_new/bash.yml                            > bash.sh
	../bin/microCI -i ../snapshot_cmd_new/docmd.yml                           > docmd.sh
	../bin/microCI -i ../snapshot_cmd_new/pandoc.yml                          > pandoc.sh
	../bin/microCI -i ../snapshot_cmd_new/template.yml                        > template.sh
	cp dot_env_minio .env.yml
	../bin/microCI -i ../snapshot_cmd_new/minio.yml                           > minio.sh
	../bin/microCI -i ../snapshot_cmd_new/jfrog.yml                           > jfrog.sh
	../bin/microCI -i ../snapshot_cmd_new/flawfinder.yml                      > flawfinder.sh
	../bin/microCI -i ../snapshot_cmd_new/docker_build.yml                    > docker_build.sh
	../bin/microCI -i ../.microCI.yml --list                     > list.txt
	../bin/microCI -i ../.microCI.yml --number 7                 > step_7.sh
	../bin/microCI -i ../.microCI.yml --number 7,9,11            > step_7_9_11_A.sh
	../bin/microCI -i ../.microCI.yml --number 11,7,9,11,9       > step_7_9_11_B.sh
	diff step_7_9_11_A.sh step_7_9_11_B.sh
	$(MAKE) -C failure_test/ test
	rm -f .env
	rm -f .env.yml
	#cat template.sh | bash

.PHONY: test
test: snapshot_create_script_test snapshot_cmd_new_test runtime_test

.PHONY: snapshot_create_script_test
snapshot_create_script_test:
	./snapshot_create_script/test_all.sh

.PHONY: snapshot_create_script_update
snapshot_create_script_update:
	./snapshot_create_script/update_expected.sh

.PHONY: snapshot_cmd_new_test runtime_test
snapshot_cmd_new_test:
	./snapshot_cmd_new/test_all.sh

.PHONY: runtime_test
runtime_test:
	./runtime/test_all.sh

#  1 05b1 C++ check with clang-tidy
#  2 ec9f C++ check with cppcheck
#  3 92ad SAST com flawfinder
#  4 9eba Convert pdf to jpg
#  5 d154 Build diagrams with plantuml
#  6 7e4a Build diagrams - pikchr
#  7 ee7b Build HTML documentation
#  8 e159 Documentation preview http://localhost:8000
#  9 cb76 Configure Github pages and README
# 10 0d3e Build PDF documentation
# 11 a99b Publish documentation to microci.dev
# 12 4680 C++ code format with clang-format
# 13 b7b7 Download project dependencies
# 14 bf1d Build microCI static version
# 15 f82a Build docker image for microci_cpp_compiler

.PHONY: test_run
test_run:
	$(MAKE) -C quick_start_01/
	$(MAKE) -C quick_start_02/
	$(MAKE) -C new_cpp/

.PHONY: clean
clean:
	rm -f .env
	rm -f .env.yml
	rm -f test_npm.yml

doctest.h:
	curl -L https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -o $@

