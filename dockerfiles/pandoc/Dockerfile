FROM pandoc/latex:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN echo "2023-01-10"; tlmgr update --self
RUN tlmgr install \
    pgf \
    tcolorbox \
    environ
