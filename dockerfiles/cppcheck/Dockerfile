FROM python:slim-bullseye
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN PIP_NO_CACHE_DIR=1 pip3 install --no-cache --upgrade pip setuptools Pygments
RUN set -eux; \
  apt-get update \
  && apt-get install -y git build-essential libpcre3-dev clang clang-format clang-tools clang-tidy \
  && mkdir -p /usr/src \
  && git clone --depth 1 --branch 2.7 --single-branch  https://github.com/danmar/cppcheck.git /usr/src/cppcheck \
  && cd /usr/src/cppcheck \
  && make install FILESDIR=/cfg HAVE_RULES=yes LDFLAGS="-lm -lpthread" CXXFLAGS="-O2 -DNDEBUG --static" -j `getconf _NPROCESSORS_ONLN` \
  && strip /usr/bin/cppcheck \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/
RUN PIP_NO_CACHE_DIR=1 pip3 install --no-cache --upgrade pip clang-html
# RUN set -eux; \
#  apt-get update \
#  && apt-get install -y libspdlog-dev
CMD [ "/bin/echo", "microCI cppcheck image" ]

# vim: ft=dockerfile
