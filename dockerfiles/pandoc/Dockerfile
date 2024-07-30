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
RUN tlmgr update --self
RUN apk add --no-cache bash font-hack-nerd font-opensans
RUN texhash && updmap-sys
RUN chmod -R o+w /opt/texlive/texdir/texmf-var \
    && fc-cache --really-force --verbose

