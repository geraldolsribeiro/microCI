FROM ruby:alpine
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

WORKDIR /microci_workspace/
COPY intmain_docmd /bin/
ENTRYPOINT ["intmain_docmd"]
