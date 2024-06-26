FROM pandoc/latex:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN tlmgr update --self \
  && tlmgr install \
    acronym \
    bigfoot \
    carlito \
    cleveref \
    collection-fontsrecommended \
    dejavu \
    enumitem \
    environ \
    etoolbox \
    everypage \
    floatrow \
    framed \
    inconsolata \
    pgf \
    sectsty \
    soul \
    tcolorbox \
    titling \
    todonotes \
    ulem \
    xargs \
    xstring
RUN apk add --no-cache bash font-hack-nerd
RUN texhash && updmap-sys

