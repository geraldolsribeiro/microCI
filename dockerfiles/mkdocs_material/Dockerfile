# MIT License
#
# Copyright (c) 2022-2025 Geraldo Luis da Silva Ribeiro
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

FROM python:slim-bookworm
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

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
