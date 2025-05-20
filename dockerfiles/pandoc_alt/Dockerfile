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

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update

# install main package to have mermaid for pandoc
RUN apt -y install pandoc
RUN apt -y install nodejs npm
RUN apt -y install texlive-latex-recommended

# install chrome dependencies
RUN apt -y install libasound2t64 libnss3 libatk1.0-0t64 libatk-bridge2.0-0t64 \
                   libcups2t64 libxdamage1 libxkbcommon0 libpango-1.0-0 libcairo2

# install mermaid filter for pandoc
RUN npm install --global mermaid-filter

# create puppeteer config for chrome.
RUN echo '{ "args": ["--no-sandbox"] }' > /root/.puppeteer.json
ENV MERMAID_FILTER_PUPPETEER_CONFIG=/root/.puppeteer.json

WORKDIR /microci

ENTRYPOINT ["pandoc"]

# FROM pandoc/latex:latest
# LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"
# RUN tlmgr update --self \
#   && tlmgr install \
#     acronym \
#     bigfoot \
#     carlito \
#     cleveref \
#     collection-fontsrecommended \
#     dejavu \
#     enumitem \
#     environ \
#     etoolbox \
#     everypage \
#     floatrow \
#     framed \
#     inconsolata \
#     pgf \
#     sectsty \
#     soul \
#     tcolorbox \
#     titling \
#     todonotes \
#     ulem \
#     xargs \
#     xstring
# RUN tlmgr update --self
# RUN apk add --no-cache bash font-hack-nerd font-opensans
# RUN texhash && updmap-sys
# RUN chmod -R o+w /opt/texlive/texdir/texmf-var \
#     && fc-cache --really-force --verbose

