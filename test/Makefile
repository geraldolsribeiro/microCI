# ░░░░░░░░░░░░░░░░░ microCI
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
# ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░ Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
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

# Para usar pushd no makefile
SHELL=bash

test:
	for i in bash clang-tidy clang-format cppcheck git_deploy git_publish mkdocs_material npm plantuml; do \
    echo "$$i" && rm -rf "new_$$i" && mkdir -p "new_$$i" \
		&& pushd "new_$$i" && pwd && ../../src/microCI -n "$$i" && popd; \
	done
	../src/microCI -i y.yml         || echo "OK"
	../src/microCI --input=x.yml    || echo "OK"
	../src/microCI -c tipo_invalido || echo "OK"
	../src/microCI                                               > default.sh
	../src/microCI -h                                            > help.txt
	../src/microCI -c npm -i test_npm.yml
	../src/microCI -i ../new/git_deploy.yml                      > git_deploy.sh
	../src/microCI -i ../new/mkdocs_material.yml                 > mkdocs_material.sh
	../src/microCI -i ../new/mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh
	../src/microCI -i ../new/cppcheck.yml                        > cppcheck.sh
	../src/microCI -i ../new/clang-tidy.yml                      > clang-tidy.sh
	../src/microCI -i ../new/clang-format.yml                    > clang-format.sh
	../src/microCI -i ../new/plantuml.yml                        > plantuml.sh
	../src/microCI -i ../new/beamer.yml                          > beamer.sh

doctest.h:
	curl -L https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -o $@

