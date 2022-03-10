# MIT License
#
# Copyright (c) 2022 Geraldo Luis da Silva Ribeiro
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

CXXFLAGS+= -DFMT_HEADER_ONLY
CXXFLAGS+= -DSPDLOG_FMT_EXTERNAL=1

CXXFLAGS+= -I../include/

CXXFLAGS+= -s
CXXFLAGS+= -O3
CXXFLAGS+= -static
CXXFLAGS+= -fPIC
CXXFLAGS+= -std=c++17
LDFLAGS+= -lstdc++fs
LDFLAGS+= -lyaml-cpp

all: deps microCI

tidy:
	clang-tidy *.cpp -- $(CXXFLAGS)

../include/new/%.hpp: ../new/%.yml
	cat .header.cpp > $@
	xxd -i $^ >> $@
	touch main.cpp

../include/new/%.hpp: ../new/%.md
	cat .header.cpp > $@
	xxd -i $^ >> $@
	touch main.cpp

../include/new/%.hpp: ../new/%.txt
	cat .header.cpp > $@
	xxd -i $^ >> $@
	touch main.cpp

../include/sh/%.hpp: ../sh/%.sh
	cat .header.cpp > $@
	xxd -i $^ >> $@
	touch main.cpp

MicroCI.cpp: \
	../include/sh/MicroCI.hpp \
	../include/sh/NotifyDiscord.hpp \
	../include/new/mkdocs_material.hpp \
	../include/new/git_deploy.hpp \
	../include/new/git_publish.hpp \
	../include/new/npm.hpp \
	../include/new/clang-format.hpp \
	../include/new/clang-format_config.hpp \
	../include/new/clang-tidy.hpp \
	../include/new/cppcheck.hpp \
	../include/new/plantuml.hpp \
	../include/new/bash.hpp \
	../include/new/mkdocs_material_config.hpp \
	../include/new/mkdocs_material_index.hpp

microCI: MicroCI.cpp main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	strip --strip-unneeded $@

deps: \
	../include/argh.hpp \
	../include/inja.hpp \
	../include/nlohmann/json.hpp

../include/argh.hpp:
	curl https://raw.githubusercontent.com/adishavit/argh/master/argh.h -o $@

../include/inja.hpp:
	curl https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp -o $@

../include/nlohmann/json.hpp:
	curl https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -o $@

clean:
	rm -f microCI
	rm -f ../include/argh.hpp
	rm -f ../include/inja.hpp
	rm -f ../include/nlohmann/json.hpp
