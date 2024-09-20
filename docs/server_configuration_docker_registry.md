# Docker Registry Server

The following `Makefile` can be used to setup a private docker registry.

```Makefile
IP=10.20.30.40

.PHONY: all
all:

.PHONY: start
start:
	docker run \
		--detach \
		--publish 5000:5000 \
		--env REGISTRY_STORAGE_DELETE_ENABLED=true \
		--volume $(shell pwd)/registry:/var/lib/registry \
		--volume $(shell pwd)/config.yml:/etc/docker/registry/config.yml \
		--restart=always \
		--name registry \
		registry:2

.PHONY: stop
stop:
	docker container stop registry || echo "Already stopped"

.PHONY: remote
remove: stop
	docker container rm -v registry

.PHONY: clean
clean:
	docker exec -it registry registry garbage-collect --delete-untagged /etc/docker/registry/config.yml

.PHONY: extract_config_yml
extract_config_yml:
	docker cp registry:/etc/docker/registry/config.yml config-default.yml

.PHONY: test
test:
	docker pull hello-world
	docker tag hello-world $(IP):5000/my-hello-world
	docker push $(IP):5000/my-hello-world
	curl http://$(IP):5000/v2/_catalog | grep "my\-hello\-world"
	docker image remove hello-world
	docker image remove $(IP):5000/my-hello-world

.PHONY: list
list:
	curl http://$(IP):5000/v2/_catalog | jq
```

## config-default.yml

A tipical config extracted by using the Makefile above has the following structure:

```yml
version: 0.1
log:
  fields:
    service: registry
storage:
  cache:
    blobdescriptor: inmemory
  filesystem:
    rootdirectory: /var/lib/registry
http:
  addr: :5000
  headers:
    X-Content-Type-Options: [nosniff]
health:
  storagedriver:
    enabled: true
    interval: 10s
    threshold: 3
```

## Access-Control headers

```yaml
http:
  headers:
    Access-Control-Allow-Origin:
    - '*'
    Access-Control-Allow-Methods:
    - GET
    - POST
    - DELETE
    - PUT
    Access-Control-Allow-Headers:
    - 'origin, authorization, accept'
```
