# MIT License
#
# Copyright (c) 2022-2026 Geraldo Luis da Silva Ribeiro
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

Q?=@

# Re: Removing unused functions/dead code
# https://gcc.gnu.org/legacy-ml/gcc-help/2003-08/msg00128.html

# Detect system
UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -m)

# Check if macOS (Darwin) and ARM (aarch64/arm64)
ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_P),arm64)
        IS_MAC_ARM = 1
    endif
endif

CXXFLAGS+= -DFMT_HEADER_ONLY
CXXFLAGS+= -DSPDLOG_FMT_EXTERNAL=1

CXXFLAGS+= -I../include/
CXXFLAGS+= -I../include/3rd/

CXXFLAGS+= -Wall -Wextra -Wpedantic -Werror

ifndef IS_MAC_ARM

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

endif

# Optimize by size
CXXFLAGS+= -Os

# CXXFLAGS+= -flto # Link Time Optimizations
CXXFLAGS+= -std=c++20
# LDLIBS+= -lstdc++fs
LDLIBS+= -lyaml-cpp
LDLIBS+= -lcrypto
LDLIBS+= -lzstd
LDLIBS+= -lz

# Mac ARM-specific settings
ifdef IS_MAC_ARM
CXXFLAGS += -I/opt/homebrew/include
LDLIBS += -L/opt/homebrew/lib
# LIBS += -lmyarmlib
endif

.PHONY: all
all: ../bin/microCI 

.PHONY: diagram
diagram:
	../bin/microCI --input ../.microCI.yml --activity-diagram > ../docs/diagrams/microCI_activity_diagram.puml

.PHONY: tidy
tidy:
	clang-tidy *.cpp -- $(CXXFLAGS)


../include/new/%.hpp: ../new/%.yml
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/new/%.hpp: ../new/%.md
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/new/%.hpp: ../new/%.txt
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/sh/%.hpp: ../sh/%.sh
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/3rd/%.hpp: ../3rd/%
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/help/%.hpp: ../help/%.txt
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

NEW_HPP=$(subst ../new/,../include/new/,$(addsuffix .hpp,$(basename $(wildcard ../new/*.*))))
HELP_HPP=$(subst ../help/,../include/help/,$(addsuffix .hpp,$(basename $(wildcard ../help/*.txt))))

UPX=upx --best
# UPX=echo

single_compilation_unit.cpp: \
	$(NEW_HPP) \
	$(HELP_HPP) \
	$(filter-out single_compilation_unit.cpp, $(wildcard *cpp)) \
	$(wildcard ../include/*hpp) \
	$(wildcard ../include/new/*hpp) \
	$(wildcard ../include/sh/*hpp) \
	Makefile \
  ../include/3rd/beamercolorthemestr.sty.hpp \
  ../include/3rd/str-logo.png.hpp
	$(Q)echo "// DON'T EDIT THIS FILE" > $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)find . -name "*.cpp" \
		| grep -v $@ \
		| grep -v .header.cpp \
		| grep -v main.cpp \
		| sed 's|\./\(.*\)|#include "\1" // NOLINT|' \
		| sort >> $@
	$(Q)echo '#include "main.cpp" // NOLINT' >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)echo "Single compilation unit was generated"

ifdef IS_MAC_ARM
../bin/microCI: single_compilation_unit.cpp
	$(Q)mkdir -p ../bin/
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
else
../bin/microCI: single_compilation_unit.cpp
	$(Q)mkdir -p ../bin/
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	$(Q)strip --strip-all $@
	$(Q)$(UPX) $@
endif

.PHONY: clean
clean:
	$(Q)$(RM) ../bin/microCI
	$(Q)touch ../new/*.yml
	$(Q)touch ../docs/*.md

.PHONY: rebuild
rebuild: clean
	$(Q)$(MAKE) -C ../docs/

.PHONY: mac_deps
mac_deps:
	brew install spdlog yaml-cpp inja

.PHONY: debian_deps
debian_deps:
	sudo apt install libyaml-cpp-dev libspdlog-dev libssl-dev libzstd-dev upx-ucl equivs devscripts git-buildpackage glow

.PHONY: header_only
header_only:
	mkdir -p ../include/3rd/nlohmann
	mkdir -p ../include/3rd/inja
	wget https://raw.githubusercontent.com/adishavit/argh/master/argh.h -O ../include/3rd/argh.h
	wget https://raw.githubusercontent.com/Rookfighter/inifile-cpp/master/include/inicpp.h -O ../include/3rd/inicpp.h
	wget https://raw.githubusercontent.com/pantor/inja/refs/heads/main/single_include/inja/inja.hpp  -O ../include/3rd/inja/inja.hpp
	wget https://raw.githubusercontent.com/nlohmann/json/refs/heads/develop/single_include/nlohmann/json.hpp -O ../include/3rd/nlohmann/json.hpp
	wget https://raw.githubusercontent.com/nlohmann/json/refs/heads/develop/single_include/nlohmann/json_fwd.hpp -O ../include/3rd/nlohmann/json_fwd.hpp

