# ░░░░░░░░░░░░░░░░░ microCI
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░ https://github.com/geraldolsribeiro/microCI
# ░░░█▀▀░▀▀▀░▀▀▀░░░ Licensed under the MIT License <http://opensource.org/licenses/MIT>.
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░ Copyright (c) 2022-2024 Geraldo Luis da Silva Ribeiro
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
	../bin/microCI -i ../new/git_deploy.yml                      > git_deploy.sh
	../bin/microCI -i ../new/mkdocs_material.yml                 > mkdocs_material.sh
	../bin/microCI -i ../new/mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh
	../bin/microCI -i ../new/cppcheck.yml                        > cppcheck.sh
	../bin/microCI -i ../new/clang-tidy.yml                      > clang-tidy.sh
	../bin/microCI -i ../new/clang-format.yml                    > clang-format.sh
	../bin/microCI -i ../new/plantuml.yml                        > plantuml.sh
	../bin/microCI -i ../new/pikchr.yml                          > pikchr.sh
	../bin/microCI -i ../new/beamer.yml                          > beamer.sh
	../bin/microCI -i ../new/fetch.yml                           > fetch.sh
	../bin/microCI -i ../new/fetch2.yml                          > fetch2.sh
	../bin/microCI -i ../new/skip.yml                            > skip.sh
	../bin/microCI -i ../new/bash.yml                            > bash.sh
	../bin/microCI -i ../new/docmd.yml                           > docmd.sh
	../bin/microCI -i ../new/pandoc.yml                          > pandoc.sh
	../bin/microCI -i ../new/template.yml                        > template.sh
	cp dot_env_minio .env.yml
	../bin/microCI -i ../new/minio.yml                           > minio.sh
	../bin/microCI -i ../new/flawfinder.yml                      > flawfinder.sh
	../bin/microCI -i ../new/docker_build.yml                    > docker_build.sh
	rm -f .env
	rm -f .env.yml
	#cat template.sh | bash

doctest.h:
	curl -L https://raw.githubusercontent.com/doctest/doctest/master/doctest/doctest.h -o $@

