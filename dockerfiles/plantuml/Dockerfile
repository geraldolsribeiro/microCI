FROM openjdk:23-slim
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

# MIT License
ARG URL=https://github.com/plantuml/plantuml/releases/download/v1.2024.4/plantuml-mit-1.2024.4.jar
# GPL stuffs are note availible: Ditaa, Jcckit, Sudoku, Xearth

# Evita a criação de uma pasta '?'
ENV _JAVA_OPTIONS=-Duser.home=/tmp/

RUN apt-get update \
  && apt-get install -y curl graphviz libxext6 libxrender1 libxext6 libxtst6 \
  && mkdir -p /opt/plantuml \
  && curl -L "${URL}" -o /opt/plantuml/plantuml.jar \
  && apt-get clean \
  && echo '#!/bin/bash\n\
java -jar /opt/plantuml/plantuml.jar "$*"\n\
' > /usr/local/bin/plantuml \
  && chmod 755 /usr/local/bin/plantuml

