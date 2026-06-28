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

FROM asciidoctor/docker-asciidoctor:main.main
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>"

# Very similar image to doxygen
# RUN echo "2026-06-28" \
#   && apk add --no-cache \
#     bash \
#     plantuml \
#     graphviz
#
# # Make same path available on Debian and Alpine
# RUN mkdir -p /usr/share/plantuml/ && ln -sf /usr/share/java/plantuml.jar /usr/share/plantuml/plantuml.jar
#
# # https://github.com/plantuml/plantuml/releases/download/v1.2025.1/plantuml-mit-1.2025.1.jar
# # https://github.com/plantuml/plantuml/releases/download/v1.2026.2/plantuml-mit-1.2026.2.jar
# # https://github.com/plantuml/plantuml/releases/download/v1.2026.2/plantuml-1.2026.2.jar
# # https://github.com/plantuml/plantuml/releases/download/v1.2026.4/plantuml-mit-1.2026.4.jar
# RUN wget https://github.com/plantuml/plantuml/releases/download/v1.2026.6/plantuml-mit-1.2026.6.jar -O /usr/share/java/plantuml.jar

WORKDIR /microci_workspace

CMD [ "/bin/echo", "microCI plantuml image" ]
