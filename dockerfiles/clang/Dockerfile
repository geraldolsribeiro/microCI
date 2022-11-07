FROM debian:bullseye-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN set -eux; \
  apt-get update \
  && apt-get install -y curl \
  && echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye main" > /etc/apt/sources.list.d/llvm.list \
  && curl https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/llvm.asc \
  && apt-get update \
  && apt-get install -y \
    git build-essential libpcre3-dev clang clang-format clang-tools clang-tidy lcov \
    xxd libspdlog-dev libyaml-cpp-dev libcrypto++-dev cmake ninja-build doxygen \
    libasio-dev libboost-all-dev \
    python3-pip \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/ \
  && PIP_NO_CACHE_DIR=1 pip3 install clang-html

RUN set -eux; \
  echo "2022-11-01" \
  && apt-get update \
  && apt-get upgrade -y \
  && apt-get clean autoclean \

CMD [ "/bin/echo", "microCI clang image" ]

# vim: ft=dockerfile
