MANIM ?= .venv/bin/manimgl
SCENES := Title Problem HowItWorks Benefits Usage Closing

.PHONY: all test preview preview-title preview-problem preview-how preview-benefits preview-usage preview-closing render clean

all: render

# Syntax/import check without opening a rendering window.
test:
	$(MANIM) --help >/dev/null
	$(MANIM) microCI.py --help >/dev/null
	python -m py_compile microCI.py

# Render all scenes at preview quality without stopping between scenes.
preview:
	@for scene in $(SCENES); do \
		$(MANIM) microCI.py $$scene -l -r 960x540; \
	done

preview-title:
	$(MANIM) microCI.py Title -l -r 960x540

preview-problem:
	$(MANIM) microCI.py Problem -l -r 960x540

preview-how:
	$(MANIM) microCI.py HowItWorks -l -r 960x540

preview-benefits:
	$(MANIM) microCI.py Benefits -l -r 960x540

preview-usage:
	$(MANIM) microCI.py Usage -l -r 960x540

preview-closing:
	$(MANIM) microCI.py Closing -l -r 960x540

# Render the complete approximately 60-second presentation.
render:
	$(MANIM) microCI.py $(SCENES) --headless -w

clean:
	rm -rf media
