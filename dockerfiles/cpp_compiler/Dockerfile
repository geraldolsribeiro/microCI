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

FROM debian:trixie-slim
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>"

RUN set -eux; \
  echo "2026-06-15" > /dev/null \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    gcc \
    g++ \
    make \
    build-essential \
    xxd \
    git \
    pkg-config \
    cmake \
    python3 \
    python3-pip \
  && apt-get clean autoclean \
  && rm -rf /var/lib/apt/lists/* \
  && echo "deb http://apt.llvm.org/trixie/ llvm-toolchain-trixie-22 main" > /etc/apt/sources.list.d/llvm.list \
  && echo "deb-src http://apt.llvm.org/trixie/ llvm-toolchain-trixie-22 main" >> /etc/apt/sources.list.d/llvm.list \
  && curl https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/llvm.asc \
  && apt-get update \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  echo "2026-03-14" > /dev/null \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    clang-22 \
    clang-format-22 \
    clang-tidy-22 \
    cppcheck \
    doxygen \
    lcov \
    ninja-build \
  && apt-get clean autoclean \
  && rm -rf /var/lib/apt/lists/*

# The package libpcre3 will not exist on trixie and the code should be ported to libpcre2.
RUN set -eux; \
  echo "2024-06-06" > /dev/null \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    libasio-dev \
    libcrypto++-dev \
    libpcre2-dev \
    libspdlog-dev \
    libyaml-cpp-dev \
  && apt-get clean autoclean \
  && rm -rf /var/lib/apt/lists/*

#  https://stackoverflow.com/questions/75608323/how-do-i-solve-error-externally-managed-environment-every-time-i-use-pip-3
RUN mv /usr/lib/python3.13/EXTERNALLY-MANAGED /usr/lib/python3.13/EXTERNALLY-MANAGED.old \
  && PIP_NO_CACHE_DIR=1 pip install --no-cache-dir clang-html

RUN set -eux; \
  echo "2025-07-22" > /dev/null \
  && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-22 100 \
  && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-22 100 \
  && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-22 100

RUN set -eux; \
  echo "2026-06-15" > /dev/null \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    apt-utils \
    gcc-arm-linux-gnueabi \
    g++-arm-linux-gnueabi \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

CMD [ "/bin/echo", "microCI C++ compiler image (x86_64)" ]

# vim: ft=dockerfile
