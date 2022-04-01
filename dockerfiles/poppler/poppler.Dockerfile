FROM alpine:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN apk --no-cache add poppler poppler-utils
