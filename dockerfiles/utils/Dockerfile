FROM alpine:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

ADD https://github.com/sclevine/yj/releases/download/v5.1.0/yj-linux-amd64 /bin/yj
RUN chmod 755 /bin/yj

RUN apk add --no-cache \
  bash \
  fontconfig \
  jq \
  make \
  msttcorefonts-installer \
  plantuml

RUN update-ms-fonts

