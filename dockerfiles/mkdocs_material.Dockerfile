FROM python:slim-bullseye
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN PIP_NO_CACHE_DIR=1 pip3 install --no-cache --upgrade pip setuptools \
  Pygments \
  mkdocs-material \
  mkdocs-minify-plugin \
  mkdocs-redirects \
  mkdocs-mermaid2-plugin

RUN PIP_NO_CACHE_DIR=1 pip3 install mkdocs-with-pdf mkdocs-build-plantuml-plugin
