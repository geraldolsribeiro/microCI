FROM python:slim-bullseye
MAINTAINER Geraldo Luis da Silva Ribeiro <geraldo@intmain.io>

# https://www.intel.com/content/www/us/en/develop/documentation/installation-guide-for-intel-oneapi-toolkits-linux/top/installation/install-using-package-managers/apt.html

RUN apt-get update \
  && apt-get install -y wget gpg \
  && wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
  | gpg --dearmor \
  | tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null \
  && echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" \
  | tee /etc/apt/sources.list.d/oneAPI.list

RUN apt-get update && apt-get install -y intel-oneapi-ipp-devel-2021.3.0 intel-oneapi-ipp-2021.3.0

