#!/bin/bash
### ----------------------------------------------------------------------
### Copyright (C) 2022-2025 Geraldo Ribeiro <geraldo@intmain.io>
### ----------------------------------------------------------------------

DOCKER_REGISTRY_IP=192.168.1.11

for image in $(docker image ls | grep microci | grep -v "$DOCKER_REGISTRY_IP" | grep -v none | sed "s/\s\+/:/; s/\s.*//")
do
	docker tag ${image} ${DOCKER_REGISTRY_IP}:5000/${image}
	docker push ${DOCKER_REGISTRY_IP}:5000/${image}
done

