FROM rust:slim-trixie
LABEL org.opencontainers.image.authors="Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>"

# Install system dependencies if required by your crate
# RUN apt-get update && apt-get install -y --no-install-recommends \
#     build-essential \
#     && rm -rf /var/lib/apt/lists/*

# Install the crate globally into /usr/local/bin
RUN cargo install cabinpkg --root /usr/local

USER 1000
WORKDIR /microci_workspace 

CMD ["/bin/bash" ]

# (Optional) Switch to a non-root user; the binary will still be executable
# USER 1000
# CMD ["<CRATE_NAME>"]

# FROM debian:stable-slim
# MAINTAINER Geraldo Luis da Silva Ribeiro <geraldolsribeiro@gmail.com>
#
# # https://github.com/orgs/cabinpkg/discussions/1052
#
# RUN set -eux; \
#   apt-get update \
#   && apt-get install -y \
#     build-essential \
#     git \
#     libcurl4-openssl-dev \
#     libfmt-dev \
#     libgit2-dev \
#     libtbb-dev \
#     nlohmann-json3-dev \
#   && apt-get clean autoclean \
#   && apt-get autoremove --yes \
#   && rm -rf /var/lib/{apt,dpkg,cache,log}/
#
# RUN apt-get update \
#   && apt-get install -y pkg-config
#
# # RUN git clone https://github.com/poac-dev/poac.git && cd poac && make RELEASE=1 install
# RUN git clone https://github.com/cabinpkg/cabin.git && cd cabin && make
#
# WORKDIR /microci_workspace 
#
# #ENTRYPOINT ["/usr/local/bin/poac" ]
# CMD ["/bin/bash" ]
