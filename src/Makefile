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
#
# ==============================================================================
# microCI - Build System Makefile
# ==============================================================================
#
# This Makefile builds the microCI tool with the following features:
# - Cross-platform compilation (Linux/macOS)
# - ARM64 and x86_64 support
# - Dead code elimination for minimal binaries
# - Single compilation unit generation from multiple source files
# - Embedded asset compilation (YAML, markdown, shell scripts as binary)
#
# Usage:
#   make          - Build the microCI tool
#   make clean    - Remove build artifacts
#   make rebuild  - Clean and rebuild, including documentation
#   make diagram  - Generate activity diagram in PlantUML format
#   make tidy     - Run clang-tidy on all C++ sources
#
# ==============================================================================
# Section 1: Variables and Platform Detection
# ==============================================================================

# Silence commands that produce no output (e.g., touch operations)
Q?=@

# Compiler flags for dead code elimination
# Reference: https://gcc.gnu.org/legacy-ml/gcc-help/2003-08/msg00128.html

# Detect system
UNAME_S := $(shell uname -s)
UNAME_P := $(shell uname -m)

# Check if macOS (Darwin) and ARM (aarch64/arm64)
ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_P),arm64)
        IS_MAC_ARM = 1
    endif
endif

# ==============================================================================
# Section 2: Compiler Flags (CXXFLAGS)
# ==============================================================================

CXXFLAGS+= -DFMT_HEADER_ONLY           # Use header-only fmt library
CXXFLAGS+= -DSPDLOG_FMT_EXTERNAL=1     # Link fmt externally instead of embedded

# Include directories for headers
CXXFLAGS+= -I../include/                # Project header directory
CXXFLAGS+= -I../include/3rd/            # Third-party library headers

# Strict compiler warnings (treat all warnings as errors)
CXXFLAGS+= -Wall -Wextra -Wpedantic -Werror

# ==============================================================================
# Section 3: Static Library Linking (Non-macOS ARM only)
# ==============================================================================

ifndef IS_MAC_ARM

# Link against static libraries.
# Required for dead-code elimination to work properly.
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
# Strip debug info to minimize binary size (for dead-code builds)
CXXFLAGS+= -s

endif

# ==============================================================================
# Section 4: General Optimization Flags
# ==============================================================================

# Optimize for minimal binary size
CXXFLAGS+= -Os # Optimize for Size
# CXXFLAGS+= -Oz # Aggressive Size Reduction
# For MacOS the binary size was 1.6MB with Os and 1.7MB with Oz

# CXXFLAGS+= -flto # Link Time Optimizations
CXXFLAGS+= -std=c++20
# Link libraries required by the project
LDLIBS+= -lyaml-cpp    # YAML parsing library
LDLIBS+= -lcrypto      # OpenSSL crypto library
LDLIBS+= -lzstd        # Zstandard compression library
LDLIBS+= -lz           # Standard zlib

# Mac ARM-specific settings
# ==============================================================================
# Section 5: macOS ARM64 Specific Settings
# ==============================================================================

ifdef IS_MAC_ARM
CXXFLAGS += -I/opt/homebrew/include   # Homebrew include paths
LDLIBS += -L/opt/homebrew/lib         # Homebrew library paths
endif

# ==============================================================================
# Section 6: Build Targets
# ==============================================================================
#
# The 'all' target is the default build target that produces ../bin/microCI.

.PHONY: all
all: ../bin/microCI  # Default target - builds the microCI executable 

.PHONY: diagram
diagram:
	../bin/microCI --input ../.microCI.yml --activity-diagram > ../docs/diagrams/microCI_activity_diagram.puml  # Generate PlantUML activity diagram

.PHONY: tidy
tidy:
	clang-tidy *.cpp -- $(CXXFLAGS)  # Run clang-tidy static analysis


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

# ==============================================================================
# Section 7: Header Generation Rules
# ==============================================================================
#
# Generated headers from YAML, Markdown, and shell scripts are created using
# the .header.cpp template. These rules embed binary content of source files
# directly into C++ headers as character arrays.
#
# Pattern matching creates:
#   - new/*.yml      → include/new/*.hpp (from YAML files)
#   - new/*.md       → include/new/*.hpp (from Markdown files)
#   - new/*.txt      → include/new/*.hpp (from text files)
#   - sh/*.sh        → include/sh/*.hpp (from shell scripts)
#   - 3rd/*          → include/3rd/*.hpp (third-party headers)
#   - external/*.yml → include/external/*.hpp (third-party headers only libraries)

../include/new/%.hpp: ../new/%.yml
	$(Q)echo "Creating $@ from $<..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

../include/sh/%.hpp: ../sh/%.sh
	$(Q)echo "Creating $@ from $<..."
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

../include/external/%.hpp: ../external/%.yml
	$(Q)echo "Creating $@..."
	$(Q)cat .header.cpp > $@
	$(Q)echo "// DON'T EDIT THIS FILE, INSTEAD UPDATE $^" >> $@
	$(Q)echo "// clang-format off" >> $@
	$(Q)xxd -i $^ | sed "s/ 0x0a/\n\n\n  0x0a/g" >> $@
	$(Q)echo "// clang-format on" >> $@
	$(Q)touch main.cpp

# ==============================================================================
# Section 8: Header Variable Generation for Embedded Assets
# ==============================================================================
#
# These variables generate header file names from embedded asset directories.
# The generated headers contain binary data of YAML, Markdown, and shell files,
# which are then included in the single compilation unit.
#
# Pattern rules transform:
#   - ../new/*.yml  → ../include/new/<name>.hpp (YAML data as C string)
#   - ../new/*.md   → ../include/new/<name>.hpp (Markdown data as C string)
#   - ../help/*.txt → ../include/help/<name>.hpp (Help text as C string)
#   - ../sh/*.sh    → ../include/sh/<name>.hpp  (Shell scripts as C string)
#
NEW_HPP=$(subst ../new/,../include/new/,$(addsuffix .hpp,$(basename $(wildcard ../new/*.*))))
HELP_HPP=$(subst ../help/,../include/help/,$(addsuffix .hpp,$(basename $(wildcard ../help/*.txt))))

# ==============================================================================
# Section 9: Build and Optimization Tools
# ==============================================================================
#
# UPX is used to compress the final binary, significantly reducing its size.
# The compressed version will automatically be used for macOS ARM builds as well.

UPX=upx --best
# UPX=echo  # Uncomment to disable UPX compression

# ==============================================================================
# Section 10: Single Compilation Unit Generation
# ==============================================================================
#
# The single_compilation_unit.cpp file aggregates all C++ sources into one
# large source file. This simplifies the build process and enables whole-
# program optimizations like link-time optimization (LTO) if desired.
#
# Dependencies include:
#   - All generated headers from new/, help/, and 3rd/ directories
#   - All *.cpp source files in the current directory
#   - The Makefile itself for proper dependency tracking
#
# Output is a single file that includes all sources via #include directives
#
# ==============================================================================

single_compilation_unit.cpp: \
	$(NEW_HPP) \
	$(HELP_HPP) \
	$(filter-out single_compilation_unit.cpp, $(wildcard *cpp)) \
	$(wildcard ../include/*hpp) \
	$(wildcard ../include/new/*hpp) \
	$(wildcard ../include/sh/*hpp) \
	$(wildcard ../include/3rd/*hpp) \
	$(wildcard ../include/external/*hpp) \
	Makefile
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

# ==============================================================================
# Section 11: Binary Build Targets
# ==============================================================================
#
# Builds the microCI executable with platform-specific optimizations:
# - Non-ARM builds are stripped and UPX compressed for minimal size
# - macOS ARM builds link without compression (no UPX support on macOS)

ifdef IS_MAC_ARM
../bin/microCI: single_compilation_unit.cpp
	$(Q)mkdir -p ../bin/
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
else
../bin/microCI: single_compilation_unit.cpp
	$(Q)mkdir -p ../bin/
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	$(Q)strip --strip-all $@  # Remove all debug symbols
	$(Q)echo "Compressing executable with UPX"
	$(Q)$(UPX) $@  # Compress binary with UPX
endif

# ==============================================================================
# Section 12: Utility Targets
# ==============================================================================

.PHONY: clean
clean:
	$(Q)$(RM) ../bin/microCI  # Remove built binary
	# Clear timestamps to force regeneration
	$(Q)touch ../new/*.yml
	$(Q)touch ../docs/*.md

.PHONY: rebuild
rebuild: clean
	$(Q)$(MAKE) -C ../docs/  # Clean and rebuild with documentation

# ==============================================================================
# Section 13: Dependency Installation Targets
# ==============================================================================

# MacOS dependency installation
# coreutils provides timeout command used in tests
.PHONY: mac_deps
mac_deps:
	brew install spdlog yaml-cpp inja glow upx coreutils # Install macOS dependencies

# Debian/Ubuntu dependency installation
.PHONY: debian_deps
debian_deps:
	sudo apt install libyaml-cpp-dev libspdlog-dev libssl-dev libzstd-dev upx-ucl equivs devscripts git-buildpackage glow

# ==============================================================================
# Section 14: Header-Only Library Setup
# ==============================================================================
#
# Downloads and sets up single-header versions of commonly used C++ libraries.
# These are pulled from their respective GitHub repositories as single include files.

.PHONY: header_only
header_only:
	mkdir -p ../include/3rd/nlohmann
	mkdir -p ../include/3rd/inja
	wget https://raw.githubusercontent.com/adishavit/argh/master/argh.h -O ../include/3rd/argh.h
	wget https://raw.githubusercontent.com/Rookfighter/inifile-cpp/master/include/inicpp.h -O ../include/3rd/inicpp.h
	wget https://raw.githubusercontent.com/pantor/inja/refs/heads/main/single_include/inja/inja.hpp  -O ../include/3rd/inja/inja.hpp
	wget https://raw.githubusercontent.com/nlohmann/json/refs/heads/develop/single_include/nlohmann/json.hpp -O ../include/3rd/nlohmann/json.hpp
	wget https://raw.githubusercontent.com/nlohmann/json/refs/heads/develop/single_include/nlohmann/json_fwd.hpp -O ../include/3rd/nlohmann/json_fwd.hpp

