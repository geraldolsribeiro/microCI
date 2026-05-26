all: 10_help.md microCI_activity_diagram.puml

10_help.md: ../bin/microCI
	echo "# Help" > $@
	echo "" >> $@
	echo "The basic usage can be obtained by passing the \`--help\` option:" >> $@
	echo "" >> $@
	echo "\`\`\`" >> $@
	echo "microCI --help" >> $@
	../bin/microCI --help >> $@
	echo "\`\`\`" >> $@

microCI_activity_diagram.puml: ../bin/microCI
	../bin/microCI --input ../.microCI.yml --activity-diagram > $@
