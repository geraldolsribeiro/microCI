FROM python:slim-bookworm
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN PIP_NO_CACHE_DIR=1 pip3 install --no-cache --upgrade pip setuptools \
  Pygments \
  mkdocs-material \
  mkdocs-minify-plugin \
  mkdocs-redirects \
  mkdocs-mermaid2-plugin \
  mkdocs-static-i18n

RUN PIP_NO_CACHE_DIR=1 pip3 install mkdocs-with-pdf mkdocs-build-plantuml-plugin

RUN apt-get update && apt-get install -y chromium-driver && apt-get clean

RUN PIP_NO_CACHE_DIR=1 pip3 install qrcode

# Demora muito para gerar um pdf para cada página
# RUN PIP_NO_CACHE_DIR=1 pip3 install mkdocs-pdf-export-plugin
