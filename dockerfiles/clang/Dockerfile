FROM debian:bookworm-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN set -eux; \
  echo "2024-02-22" \
  && apt-get update \
  && apt-get install -y curl \
  && echo "deb http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm main" > /etc/apt/sources.list.d/llvm.list \
  && curl https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/llvm.asc \
  && apt-get update \
  && apt-get install -y \
    build-essential \
    clang \
    clang-format \
    clang-tidy \
    clang-tools \
    cmake \
    crossbuild-essential-arm64 \
    doxygen \
    git \
    lcov \
    libasio-dev \
    libboost-all-dev \
    libcrypto++-dev \
    libpcre3-dev \
    libspdlog-dev \
    libyaml-cpp-dev \
    ninja-build \
    python3-pip \
    xxd \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/
RUN apt-get install -y python3-full
# RUN PIP_NO_CACHE_DIR=1 pip3 --break-system-packages install clang-html

CMD [ "/bin/echo", "microCI clang image" ]

# vim: ft=dockerfile
