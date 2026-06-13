PLUGINS_TXT=$(addprefix ../help/,$(subst .md,.txt,$(subst _plugin_,,$(wildcard ???_plugin_*.md))))

.PHONY: all
all: \
	$(PLUGINS_TXT) \
	120_help.md \
	microCI_activity_diagram.puml

GLOW_STYLE=tokyo-night
GLOW_STYLE=dracula

../help/%.txt: ???_plugin_%.md
	CLICOLOR_FORCE=1 glow --width 100 --style $(GLOW_STYLE) $< > $@

../bin/microCI:
	$(MAKE) -C ../src/

120_help.md: ../bin/microCI
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

