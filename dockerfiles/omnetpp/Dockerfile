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

FROM ubuntu:noble
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

RUN set -eux; \
  echo "2024-09-25" > /dev/null \
  && apt-get update \
  && apt-get install -y \
    bison \
    build-essential \
    ca-certificates \
    ccache \
    clang \
    clang-format \
    clang-tidy \
    clang-tools \
    cmake \
    curl \
    doxygen \
    flex \
    gdb \
    gettext \
    git \
    graphviz \
    lcov \
    libfl-dev \
    libopenscenegraph-dev \
    libxml2-dev \
    lld \
    make \
    perl \
    pkg-config \
    python3 \
    python3-matplotlib \
    python3-numpy \
    python3-pandas \
    python3-pip \
    python3-scipy \
    python3-seaborn \
    qt5-qmake \
    qtbase5-dev \
    qtbase5-dev-tools \
    qtchooser \
    wget \
    vim \
    xxd \
    zlib1g-dev \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/apt /var/lib/dpkg /var/lib/cache/ /var/lib/log/

RUN pip install --no-cache-dir --break-system-packages posix-ipc 

WORKDIR /root

ARG OMNETPP_VERSION=6.1rc1
RUN wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-${OMNETPP_VERSION}/omnetpp-${OMNETPP_VERSION}-core.tgz \
         -O omnetpp-${OMNETPP_VERSION}-core.tgz --progress=dot:giga \
    && tar xf omnetpp-${OMNETPP_VERSION}-core.tgz \
    && rm omnetpp-${OMNETPP_VERSION}-core.tgz \
    && mv omnetpp-${OMNETPP_VERSION} omnetpp-${OMNETPP_VERSION}-linux

WORKDIR /root/omnetpp-${OMNETPP_VERSION}-linux

ENV PATH /root/omnetpp-${OMNETPP_VERSION}-linux/bin:$PATH
ENV __omnetpp_root_dir /root/omnetpp-${OMNETPP_VERSION}-linux
ENV OMNETPP_RELEASE omnetpp-${OMNETPP_VERSION}

RUN ./configure WITH_QTENV=yes WITH_OSG=yes WITH_OSGEARTH=no WITH_LIBXML=yes
RUN make -j "$(nproc --ignore=2)"

CMD [ "/bin/echo", "OMNET++ Ubuntu 24.04 x86_64 image" ]
