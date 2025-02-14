FROM debian:stable-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN set -eux; \
  apt-get update \
  && apt-get install -y \
    build-essential \
    git \
    libcurl4-openssl-dev \
    libfmt-dev \
    libgit2-dev \
    libtbb-dev \
    nlohmann-json3-dev \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

RUN apt-get update \
  && apt-get install -y pkg-config

RUN git clone https://github.com/poac-dev/poac.git && cd poac && make RELEASE=1 install
WORKDIR /microci_workspace 

#ENTRYPOINT ["/usr/local/bin/poac" ]
CMD ["/bin/bash" ]
