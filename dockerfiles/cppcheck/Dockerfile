FROM debian:testing-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN set -eux; \
  apt-get update \
  && apt-get install -y cppcheck \
  && apt-get clean autoclean \
  && apt-get autoremove --yes \
  && rm -rf /var/lib/{apt,dpkg,cache,log}/

CMD [ "/bin/echo", "microCI cppcheck image" ]
