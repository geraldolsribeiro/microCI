# Docker Registry UI

## `docker-compose.yml` example

```yml
services:
  registry-ui:
    image: joxit/docker-registry-ui:main
    restart: always
    ports:
      - 5001:80
    environment:
      - SINGLE_REGISTRY=true
      - REGISTRY_TITLE=My Company Docker Registry UI
      - DELETE_IMAGES=true
      - SHOW_CONTENT_DIGEST=true
      - NGINX_PROXY_PASS_URL=http://10.20.30.40:5000
      - SHOW_CATALOG_NB_TAGS=true
      - CATALOG_MIN_BRANCHES=1
      - CATALOG_MAX_BRANCHES=1
      - TAGLIST_PAGE_SIZE=100
      - REGISTRY_SECURED=false
      - CATALOG_ELEMENTS_LIMIT=1000
    container_name: registry-ui
```

## `Makefile` example

```Makefile
.PHONY: all
all:

.PHONY: start
start:
	docker compose up --detach

.PHONY: stop
stop:
	docker compose down

.PHONY: restart
restart: stop start

.PHONY: upgrade
upgrade:
	docker pull joxit/docker-registry-ui:main
```