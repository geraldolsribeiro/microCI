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

# Based on https://raw.githubusercontent.com/maxime-esa/asn1scc/master/Dockerfile.runtime

FROM  mcr.microsoft.com/dotnet/sdk:7.0 AS builder
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

RUN set -eux; \
  echo "2024-08-14" > /dev/null \
  && apt-get update \
  && apt-get upgrade --yes \
  && apt-get install --yes \
    curl \
    python3 \
    python3-distutils \
    make \
    openjdk-11-jre \
    nuget \
    libgit2-dev \
    libssl-dev \
    git \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

WORKDIR /src
RUN set -eux; \
  echo "2024-08-14" > /dev/null \
  && git clone https://github.com/maxime-esa/asn1scc.git \
  && cd asn1scc/ \
  && dotnet build Antlr/        --configuration Release \
  && dotnet build parseStg2/    --configuration Release \
  && dotnet build "asn1scc.sln" --configuration Release 


FROM mcr.microsoft.com/dotnet/runtime:7.0
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>"

COPY --from=builder /src/asn1scc/asn1scc/bin/Release/net7.0/ /opt/asn1scc
#WORKDIR /tmp
WORKDIR /microci_workspace 

# ENTRYPOINT ["/opt/asn1scc/asn1scc"]
CMD ["/bin/bash" ]
