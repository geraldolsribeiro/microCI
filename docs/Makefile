PLUGINS_TXT=$(addprefix ../help/,$(subst .md,.txt,$(subst 20_plugin_,,$(wildcard 20_plugin_*.md))))

.PHONY: all
all: \
	$(PLUGINS_TXT) \
	10_help.md \
	microCI_activity_diagram.puml

GLOW_STYLE=tokyo-night
GLOW_STYLE=dracula

../help/%.txt: 20_plugin_%.md
	CLICOLOR_FORCE=1 glow --width 100 --style $(GLOW_STYLE) $< > $@

../bin/microCI:
	$(MAKE) -C ../src/

10_help.md: ../bin/microCI
	echo "# microCI Help" > $@
	echo "" >> $@
	echo "The basic usage can be obtained by passing the \`--help\` option:" >> $@
	echo "" >> $@
	echo "\`\`\`" >> $@
	echo "microCI --help" >> $@
	../bin/microCI --help >> $@
	echo "\`\`\`" >> $@

microCI_activity_diagram.puml: ../bin/microCI
	../bin/microCI --input ../.microCI.yml --activity-diagram > $@

