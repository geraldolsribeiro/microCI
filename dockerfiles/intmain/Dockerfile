# syntax=docker/dockerfile:1.6
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

FROM debian:bookworm-slim

LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>"

# ADD https://github.com/sclevine/yj/releases/download/v5.1.0/yj-linux-amd64 /bin/yj
# ADD https://github.com/sclevine/yj/releases/latest/download/yj-linux-amd64 /bin/yj
# RUN chmod 755 /bin/yj

RUN --mount=type=cache,target=/var/cache/apt \
    --mount=type=cache,target=/var/lib/apt/lists \
    apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  ack \
  apt-utils \
  bat \
  bash \
  boxes \
  build-essential \
  ccache \
  cccc \
  checkstyle \
  clang \
  clang-format \
  cloc \
  cmake \
  cppcheck \
  cucumber \
  curl \
  detox \
  doctest-dev \
  dos2unix \
  doxygen \
  equivs \
  expect \
  flawfinder \
  fonts-noto-color-emoji \
  g++ \
  gawk \
  gcovr \
  gifsicle \
  git \
  git-flow \
  git-gui \
  graphviz \
  imagemagick \
  inkscape \
  jpegoptim \
  jq \
  libarchive-dev \
  libcrypto++-dev \
  libev-dev \
  libfontconfig1-dev \
  libicu-dev \
  libidn11-dev \
  libidn2-0-dev \
  libjsoncpp-dev \
  libmarkdown2-dev \
  libmemcached-dev \
  libncurses5-dev \
  libopenoffice-oodoc-perl \
  libopencv-dev \
  libpng++-dev \
  libpq-dev \
  libqrencode-dev \
  librocksdb-dev \
  librsvg2-bin \
  libssl-dev \
  libswitch-perl \
  libtclap-dev \
  libtiff5-dev \
  libxcursor-dev \
  libxfixes-dev \
  libxft-dev \
  libxinerama-dev \
  libxrender-dev \
  libyaml-cpp-dev \
  libyaml-perl \
  libzipios++-dev \
  libzmq3-dev \
  lintian \
  make \
  nodejs \
  npm \
  optipng \
  plantuml \
  python-is-python3 \
  ranger \
  ruby \
  ruby-dev \
  rocksdb-tools \
  shellcheck \
  sloccount \
  uuid-dev \
  w3m \
  webp \
  xmlstarlet \
  xmlto \
  xsltproc \
  yui-compressor \
  yamllint \
  && rm -rf /var/lib/apt/lists/*

RUN npm install -g \
  @mryhryki/markdown-preview \
  coffeescript \
  coffeescript-concat \
  codeceptjs \
  compass-sass-mixins \
  css-combine \
  htmllint \
  htmllint-cli \
  http-server \
  js-beautify \
  js2coffee \
  jshint \
  sass \
  sass-migrator \
  svgo \
  terser \
  vue-cli \
  web-push \
  yarn

RUN gem install \
  cucumber \
  html-proofer \
  mdl \
  syntax --no-document

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  fakeroot \
  sudo \
  && rm -rf /var/lib/apt/lists/*

RUN useradd -m -s /bin/bash user
ENV HOME=/home/user
USER user

CMD ["bash"]
