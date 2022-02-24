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

test:
	../src/microCI -i y.yml      || echo "OK"
	../src/microCI --input=x.yml || echo "OK"
	../src/microCI -c tipo_invalido || echo "OK"
	../src/microCI -h                                            > help.txt
	../src/microCI                                               > default.sh
	../src/microCI -i ../new/git_deploy.yml                      > git_deploy.sh
	../src/microCI -i ../new/mkdocs_material.yml                 > mkdocs_material.sh
	../src/microCI -i ../new/mkdocs_material.yml -O "local-step" > mkdocs_material_only.sh
	../src/microCI -c npm -i test_npm.yml
	../src/microCI -i ../new/cppcheck.yml                        > cppcheck.sh
	../src/microCI -i ../new/clang-tidy.yml                      > clang-tidy.sh
