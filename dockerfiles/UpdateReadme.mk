ifneq ($(wildcard ../.env),)
include ../.env

UPDATE_README_IMAGE=peterevans/dockerhub-description:4

publish::
	docker run --rm -v $(shell pwd):/workspace \
  	-e DOCKERHUB_USERNAME=$(DOCKERHUB_USERNAME) \
  	-e DOCKERHUB_PASSWORD=$(DOCKERHUB_TOKEN) \
  	-e DOCKERHUB_REPOSITORY=intmain/$(IMAGE_NAME) \
  	-e README_FILEPATH=/workspace/README.md \
		$(UPDATE_README_IMAGE)
endif
