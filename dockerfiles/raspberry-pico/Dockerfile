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

ENV DEBIAN_FRONTEND=noninteractive

# ARM: GCC version 9 onwards for RP2350 since that is the first version that supports the Arm Cortex-M33

RUN set -eux; \
  echo "2026-06-20" > /dev/null \
  && apt-get update \
  && apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    autotools-dev \
    bc \
    bison \
    build-essential \
    ca-certificates \
    cmake \
    curl \
    flex \
    gawk \
    gcc-arm-none-eabi \
    git \
    gperf \
    libexpat-dev \
    libglib2.0-dev \
    libgmp-dev \
    libmpc-dev \
    libmpfr-dev \
    libnewlib-arm-none-eabi \
    libslirp-dev \
    libstdc++-arm-none-eabi-newlib \
    libtool \
    libusb-1.0-0-dev \
    make \
    ninja-build \
    patchutils \
    pkg-config \
    python3 \
    python3-pip \
    texinfo \
    xxd \
    zlib1g-dev \
  && rm -rf /var/lib/apt/lists/*

# Official Debian and Ubuntu base images come pre-configured with a hook
# (docker-clean) that automatically runs apt-get clean behind the scenes.
# Manually including it is redundant

# Clone the Raspberry Pi Pico SDK
WORKDIR /opt
RUN git clone -b master --depth=1 https://github.com/raspberrypi/pico-sdk.git \
    && cd pico-sdk \
    && git submodule update --init

# Set the mandatory environment variable for the build system
ENV PICO_SDK_PATH=/opt/pico-sdk

RUN curl -L https://github.com/raspberrypi/pico-sdk-tools/releases/download/v2.2.0-3/picotool-2.2.0-a4-x86_64-lin.tar.gz \
  | tar -xzf -
ENV picotool_DIR=/opt/picotool

# Set workspace directory for your project source code
WORKDIR /microci_workspace

CMD [ "/bin/echo", "microCI C++ compiler" ]

# vim: ft=dockerfile
