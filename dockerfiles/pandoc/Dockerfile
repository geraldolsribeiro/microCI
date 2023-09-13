FROM pandoc/latex:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN tlmgr update --self \
  && tlmgr install \
    pgf \
    tcolorbox \
    environ \
    collection-fontsrecommended
RUN updmap-sys
