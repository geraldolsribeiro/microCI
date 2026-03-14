# MIT License
#
# Copyright (c) 2022-2026 Geraldo Luis da Silva Ribeiro
#
# ░░░░░░░░░░░░░░░░░
# ░░░░░░░█▀▀░▀█▀░░░
# ░░░█░█░█░░░░█░░░░
# ░░░█▀▀░▀▀▀░▀▀▀░░░
# ░░░▀░░░░░░░░░░░░░
# ░░░░░░░░░░░░░░░░░
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

.PHONY: install-docker
install-docker:
	sudo apt remove $(shell dpkg --get-selections docker.io docker-compose docker-doc podman-docker containerd runc | cut -f1)
	sudo apt update
	sudo apt install ca-certificates curl
	sudo install -m 0755 -d /etc/apt/keyrings
	sudo curl -fsSL https://download.docker.com/linux/debian/gpg -o /etc/apt/keyrings/docker.asc
	sudo chmod a+r /etc/apt/keyrings/docker.asc
	echo "Types: deb" | sudo tee /etc/apt/sources.list.d/docker.sources
	echo "URIs: https://download.docker.com/linux/debian" | sudo tee -a /etc/apt/sources.list.d/docker.sources
	echo "Suites: trixie" | sudo tee -a /etc/apt/sources.list.d/docker.sources
	echo "Components: stable" | sudo tee -a /etc/apt/sources.list.d/docker.sources
	echo "Signed-By: /etc/apt/keyrings/docker.asc" | sudo tee -a /etc/apt/sources.list.d/docker.sources
	sudo apt install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
	sudo usermod -aG docker $(USER)
