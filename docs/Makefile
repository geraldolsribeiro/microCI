all: 00_help.md

00_help.md: ../bin/microCI
	echo "# Help" > $@
	echo "" >> $@
	echo "The basic usage can be obtained by passing the `--help` option:" >> $@
	echo "" >> $@
	echo "\`\`\`" >> $@
	echo "microCI --help" >> $@
	../bin/microCI --help >> $@
	echo "\`\`\`" >> $@
