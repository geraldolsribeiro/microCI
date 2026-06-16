ifneq ($(wildcard ../.env),)
include ../.env

# .env sample:
# DOCKERHUB_USERNAME=geraldolsribeiro
# DOCKERHUB_TOKEN=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

publish::
	docker run --rm -v $(shell pwd):/workspace \
  	-e DOCKERHUB_USERNAME=$(DOCKERHUB_USERNAME) \
  	-e DOCKERHUB_PASSWORD=$(DOCKERHUB_TOKEN) \
  	-e DOCKERHUB_REPOSITORY=intmain/$(IMAGE_NAME) \
  	-e README_FILEPATH=/workspace/README.md \
		peterevans/dockerhub-description:4
endif
