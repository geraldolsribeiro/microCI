# MIT License
#
# Copyright (c) 2022-2024 Geraldo Luis da Silva Ribeiro
#
# ░░░░░░░░░░░░░░░░░
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░
# ░░░█▀▀░▀▀▀░▀▀▀░░░
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Re: Removing unused functions/dead code
# https://gcc.gnu.org/legacy-ml/gcc-help/2003-08/msg00128.html

CXXFLAGS+= -DFMT_HEADER_ONLY
CXXFLAGS+= -DSPDLOG_FMT_EXTERNAL=1

CXXFLAGS+= -I../include/

CXXFLAGS+= -Wall -Wextra

# Link against static libraries.
# Required for dead-code elimination.
CXXFLAGS+= -static

# C++ virtual method table instrumented with garbage collection information for the linker.
# CXXFLAGS+= -fvtable-gc

# Keeps data in separate data sections, so they can be discarded if unused.
CXXFLAGS+= -fdata-sections

# Keeps funcitons in separate data sections, so they can be discarded if unused.
CXXFLAGS+= -ffunction-sections

# Tell the linker to garbage collect and discard unused sections.
CXXFLAGS+= -Wl,--gc-sections

# Strip the debug information, so as to make the code as small as possible.
# (I presume that you'd want to do this in a dead-code removal build.)
CXXFLAGS+= -s

# Optimize by size
CXXFLAGS+= -Os

CXXFLAGS+= -std=c++17
LDFLAGS+= -lstdc++fs
LDFLAGS+= -lyaml-cpp
LDFLAGS+= -lcrypto

all: ../locales/pt_BR/microci.mo ../bin/microCI 
	../bin/microCI --input ../.microCI.yml --activity-diagram > ../docs/diagrams/microCI_activity_diagram.puml

tidy:
	clang-tidy *.cpp -- $(CXXFLAGS)

../locales/microci.pot: ../src/*.cpp
	xgettext $^ \
		--keyword="_" \
		--output="$@" \
		--language=C++ \
		--add-comments \
		--sort-output \
		--join-existing \
		--copyright-holder="Geraldo Luis da Silva Ribeiro" \
		--package-name="microci" \
		--package-version="$(shell sed -n "s/#define microCI_version \"\(.*\)\"/\1/p" ../include/MicroCI.hpp)" \
		--msgid-bugs-address="geraldolsribeiro@gmail.com" \

../locales/pt_BR/microci.po: ../locales/microci.pot
	msgmerge $@ $^ --output-file=$@

../locales/pt_BR/microci.mo: ../locales/pt_BR/microci.po
	msgfmt $^ --output-file=$@

test_ptBR:
	sudo cp ../locales/pt_BR/microci.mo /usr/share/locale/pt_BR/LC_MESSAGES/
	LC_ALL=pt_BR.UTF-8 ../bin/microCI

../include/new/%.hpp: ../new/%.yml
	cat .header.cpp > $@
	echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	clang-format -i $@
	touch main.cpp

../include/new/%.hpp: ../new/%.md
	cat .header.cpp > $@
	echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	clang-format -i $@
	touch main.cpp

../include/new/%.hpp: ../new/%.txt
	cat .header.cpp > $@
	echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	clang-format -i $@
	touch main.cpp

../include/sh/%.hpp: ../sh/%.sh
	cat .header.cpp > $@
	echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	clang-format -i $@
	touch main.cpp

../include/3rd/%.hpp: ../3rd/%
	cat .header.cpp > $@
	echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	clang-format -i $@
	touch main.cpp

single_compilation_unit.cpp: \
	$(filter-out single_compilation_unit.cpp, $(wildcard *cpp)) \
	$(wildcard ../include/*hpp) \
	$(wildcard ../include/new/*hpp) \
	$(wildcard ../include/sh/*hpp) \
	Makefile \
  ../include/3rd/beamercolorthemestr.sty.hpp \
  ../include/3rd/str-logo.png.hpp
	@echo "// DON'T EDIT THIS FILE" > $@
	@echo "// clang-format off" >> $@
	@find . -name "*.cpp" \
		| grep -v $@ \
		| grep -v main.cpp \
		| sed 's|\./\(.*\)|#include "\1" // NOLINT|' >> $@
	@echo '#include "main.cpp" // NOLINT' >> $@
	@echo "// clang-format on" >> $@
	@echo "Single compilation unit was generated"

../bin/microCI: single_compilation_unit.cpp
	mkdir -p ../bin/
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	strip --strip-all $@

clean:
	rm -f ../bin/microCI
	touch ../new/*.yml
