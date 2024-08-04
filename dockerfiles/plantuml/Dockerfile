# MIT License
#
# Copyright (c) 2022-2024 Geraldo Luis da Silva Ribeiro
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

FROM openjdk:23-slim
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

# Evita a criação de uma pasta '?'
ENV _JAVA_OPTIONS=-Duser.home=/tmp/

RUN apt-get update \
  && apt-get install -y curl graphviz libxext6 libxrender1 libxext6 libxtst6 \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

# MIT License
ARG URL=https://github.com/plantuml/plantuml/releases/download/v1.2024.6/plantuml-mit-1.2024.6.jar
# GPL stuffs are note availible: Ditaa, Jcckit, Sudoku, Xearth

RUN mkdir -p /opt/plantuml \
  && curl -L "${URL}" -o /opt/plantuml/plantuml.jar \
  && echo '#!/bin/bash\n\
java -jar /opt/plantuml/plantuml.jar "$*"\n\
' > /usr/local/bin/plantuml \
  && chmod 755 /usr/local/bin/plantuml

