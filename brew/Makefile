.PHONY: build
build:
	HOMEBREW_NO_INSTALL_FROM_API=1 brew install --debug --verbose --build-from-source geraldolsribeiro/tap/microci
#	HOMEBREW_NO_INSTALL_FROM_API=1 brew install --build-from-source microci

.PHONY: install
install:
	brew install --debug --verbose geraldolsribeiro/tap/microci
	# brew install geraldolsribeiro/tap/microci

.PHONY: edit
edit:
	vi /opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/microci.rb
	# vi /opt/homebrew/Library/Taps/homebrew/homebrew-core/Formula/m/microci.rb
	# vi /opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/geraldolsribeiro-microci.rb

.PHONY: audit
audit:
	brew audit --new --formula geraldolsribeiro/tap/microci

.PHONY: cpFormulaToSystem
cpFormulaTo:
	cp microci.rb /opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/
	# cp microci.rb /opt/homebrew/Library/Taps/homebrew/homebrew-core/Formula/m/microci.rb

.PHONY: lazygit
lazygit:
	pushd /opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/ && lazygit

.PHONY: cpFormulaFromSystem
cpFormulaFrom:
	cp /opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/microci.rb microci.rb
	# cp /opt/homebrew/Library/Taps/homebrew/homebrew-core/Formula/m/microci.rb microci.rb

# FORMULA=/opt/homebrew/Library/Taps/geraldolsribeiro/homebrew-tap/Formula/microci.rb
# FORMULA_URL=$(shell sed -n "s/  url \"\(.*\)\"/\1/p" $(FORMULA))
# FORMULA_SHA256=$(shell curl $(FORMULA_URL) | sha256sum | head -c 64)
# update_sha256:
# 	echo $(FORMULA_URL)
# 	echo $(FORMULA_SHA256)
# 	#sed -i "s#^  sha256 .*#  sha256 \"$(FORMULA_SHA256)\"#" $(FORMULA)

