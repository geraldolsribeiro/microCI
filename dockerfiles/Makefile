SUBDIRS := $(wildcard */.)
# SUBDIRS := clang

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)
