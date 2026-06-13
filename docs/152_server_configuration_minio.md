# MinIO Server

A MinIO Server can be deployed by docker container.

Consider the following `Makefile` as a starting point setting up your server.
Securing the server is not covered in this topic.

## Makefile

```Makefile
DOCKER_REGISTRY_IP=10.20.30.40

ifneq (,$(wildcard ./.env))
	include .env
	export
endif

.PHONY: all
all:

.PHONY: upgrade
upgrade:
	docker pull minio/minio
	docker tag  minio/minio $(DOCKER_REGISTRY_IP):5000/minio/minio
	docker push $(DOCKER_REGISTRY_IP):5000/minio/minio

.PHONY: start
start:
	docker run \
		--detach \
		--restart=always \
		--publish 9000:9000 \
		--publish 9001:9001 \
		--volume $(shell pwd)/data:/data \
		--env MINIO_ROOT_USER=$(MINIO_ROOT_USER) \
		--env MINIO_ROOT_PASSWORD=$(MINIO_ROOT_PASSWORD) \
		--name minio \
		minio/minio \
		server /data \
		--console-address ":9001"

.PHONY: stop
stop:
	docker container stop minio  || echo "Already stopped"
	docker container rm -v minio || echo "Already removed"

.PHONY: remove
remove: stop
	docker container rm -v minio || echo "Already removed"

.env:
	echo "MINIO_ROOT_USER=minioadmin" > $@
	echo "MINIO_ROOT_PASSWORD=$(shell pwgen 12 1)" >> $@
```

## .gitlab-ci.yml

The following config can be used to upgrade the server.

```yaml
before_script:
  - echo apt-get update -qq
  - echo apt-get install -qq git
  # Setup SSH deploy keys
  - 'which ssh-agent || ( apt-get install -qq openssh-client )'
  - eval $(ssh-agent -s)
  - ssh-add <(echo "$SSH_PRIVATE_KEY")
  - mkdir -p ~/.ssh
  - grep -q 10.20.30.40 ~/.ssh/known_host || ssh-keyscan -t rsa 10.20.30.40 >> ~/.ssh/known_hosts
  - '[[ -f /.dockerenv ]] && echo -e "Host *\n\tStrictHostKeyChecking no\n\n" > ~/.ssh/config'
    
deploy:
  environment: production
  script:
    - ssh admin@10.20.30.40 "cd /opt/docker/minio && git checkout main && git pull origin main && make upgrade && make stop && make start && exit"
  only:
    - main
```