# MIT License
#
# Copyright (c) 2022-2025 Geraldo Luis da Silva Ribeiro
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

FROM gcc:latest
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

RUN set -eux; \
  echo "2025-07-22" > /dev/null \
  && apt-get update \
  && apt-get install -y curl \
  && echo "deb http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm main" > /etc/apt/sources.list.d/llvm.list \
  && curl https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/llvm.asc \
  && apt-get update \
  && apt-get upgrade --yes \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  echo "2024-06-06" > /dev/null \
  && apt-get update \
  && apt-get install -y \
    build-essential \
    clang-22 \
    clang-format-22 \
    clang-tidy-22 \
    clang-tools-22 \
    cmake \
    cppcheck \
    crossbuild-essential-arm64 \
    doxygen \
    gettext \
    git \
    lcov \
    xxd \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  echo "2024-06-06" > /dev/null \
  && apt-get update \
  && apt-get install -y \
    libasio-dev \
    libcrypto++-dev \
    libpcre3-dev \
    libspdlog-dev \
    libyaml-cpp-dev \
    ninja-build \
    python3-full \
    python3-pip \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

#  https://stackoverflow.com/questions/75608323/how-do-i-solve-error-externally-managed-environment-every-time-i-use-pip-3
RUN mv /usr/lib/python3.11/EXTERNALLY-MANAGED /usr/lib/python3.11/EXTERNALLY-MANAGED.old \
  && PIP_NO_CACHE_DIR=1 pip install clang-html

RUN set -eux; \
  echo "2024-06-06" > /dev/null \
  && apt-get update \
  && apt-get upgrade -y \
  && apt-get install -y \
    apt-utils \
    gcc-arm-linux-gnueabi \
    g++-arm-linux-gnueabi \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  echo "2025-07-22" > /dev/null \
  && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-22 100 \
  && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-22 100 \
  && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-22 100

CMD [ "/bin/echo", "microCI C++ compiler image" ]

# Note: libboost-all-dev was removed due some issue related to gfortran dependencies
# Maybe more useful add libraries individually instead all one
#   Depends: libboost-dev
#   Depends: libboost-tools-dev
#     libboost-tools-dev:i386
#   Depends: libboost-atomic-dev
#   Depends: libboost-chrono-dev
#   Depends: libboost-container-dev
#   Depends: libboost-context-dev
#   Depends: libboost-coroutine-dev
#   Depends: libboost-date-time-dev
#   Depends: libboost-exception-dev
#   Depends: libboost-fiber-dev
#   Depends: libboost-filesystem-dev
#   Depends: libboost-graph-dev
#   Depends: libboost-graph-parallel-dev
#   Depends: libboost-iostreams-dev
#   Depends: libboost-locale-dev
#   Depends: libboost-log-dev
#   Depends: libboost-math-dev
#   Depends: libboost-mpi-dev
#   Depends: libboost-mpi-python-dev
#   Depends: libboost-numpy-dev
#   Depends: libboost-program-options-dev
#   Depends: libboost-python-dev
#   Depends: libboost-random-dev
#   Depends: libboost-regex-dev
#   Depends: libboost-serialization-dev
#   Depends: libboost-stacktrace-dev
#   Depends: libboost-system-dev
#   Depends: libboost-test-dev
#   Depends: libboost-thread-dev
#   Depends: libboost-timer-dev
#   Depends: libboost-type-erasure-dev
#   Depends: libboost-wave-dev
#   Depends: libboost-nowide-dev

# vim: ft=dockerfile
