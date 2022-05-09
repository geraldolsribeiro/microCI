FROM debian:bullseye-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN set -eux; \
  apt-get update \
  && apt-get install -y git build-essential libpcre3-dev clang clang-format clang-tools clang-tidy lcov \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  apt-get update \
  && apt-get install -y xxd libspdlog-dev libyaml-cpp-dev libcrypto++-dev cmake ninja-build \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  apt-get update \
  && apt-get install -y doxygen \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  apt-get update \
  && apt-get install -y libasio-dev libboost-all-dev \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN set -eux; \
  apt-get update \
  && apt-get install -y python3-pip \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/ \
  && PIP_NO_CACHE_DIR=1 pip3 install clang-html

CMD [ "/bin/echo", "microCI clang image" ]

# vim: ft=dockerfile
