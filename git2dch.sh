#!/bin/bash

# git log --no-walk --tags --format="%d %h %s  "

TAGS=$(git tag 2>/dev/null | grep -E '^v?[0-9]+\.[0-9]+(\.[0-9]+)?$' | sort -rV)

prev_tag="HEAD"
for tag in $TAGS; do
  echo "microci ($tag) unstable; urgency=low"
  git log --format="  * %s" $tag..$prev_tag
  prev_tag=${tag}
  echo
  git log $tag --oneline --no-walk --format=" -- %aN <%aE>  %aD"
  echo
done

# microci (0.43.0+snapshot20260526) unstable; urgency=low
#
#   * Create debian package
#   * Update documentation
#   * Update documentation
#   * Translate text
#   * Translate text
#   * refact: update generated code
#   * refact: modernize code
#   * Validate command line options
#   * Disable gettext translation macro
#   * Add UPX to debian_deps makefile target
#   * translate YAML from Portuguese to English
#   * improve error message in BashPluginStepParser
#   * refact: correct English texts
#   * refact: format sh
#
#  -- Geraldo Ribeiro <geraldolsribeiro@gmail.com>  Tue, 26 May 2026 16:42:24 +0400microci (0.43.0) unstable; urgency=low
#
#   * Bump version; update documentation
#   * feat: jfrog plugin
#   * fix: bitnamilegacy
#   * feat: add install.sh script
#   * Add code copy to mkdocs documentation
#   * Update brew recipe
#   * Compress binary using upx
#   * refact: update README
#   * feat: add mermaid plugin
#   * refact: update asn1
#   * refact: update cpp_compiler
#   * refact: update cppcheck docker image
