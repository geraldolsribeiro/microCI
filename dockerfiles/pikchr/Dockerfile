FROM alpine:latest
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

RUN apk add --no-cache rsvg-convert bash inkscape
RUN mkdir -p /.config/inkscape && chmod 777 /.config/inkscape

WORKDIR /microci_workspace/
COPY pikchr /bin/
CMD ["bash"]
