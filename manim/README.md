# Project animations with ManimGL

This directory contains the presentation animations for the project, created with [ManimGL](https://github.com/MathItYT/manimgl).

## Requirements

- Python 3.10 or newer
- Git
- FFmpeg
- Cairo, Pango, and PortAudio development libraries

On Debian/Ubuntu, install the system dependencies with:

```bash
sudo apt-get update
sudo apt-get install -y pkg-config libcairo2-dev libpango1.0-dev portaudio19-dev ffmpeg
```

## Create the environment

Create a local Python virtual environment from this directory:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install 'git+https://github.com/MathItYT/manimgl.git'
```

The environment is stored in `.venv/` and should not be committed. The setup has been verified with Python 3.13 and ManimGL 1.7.2. To use it again later:

```bash
source .venv/bin/activate
```

To leave the environment:

```bash
deactivate
```

## Create an animation

Create a Python file, for example `presentation.py`:

```python
from manimlib import *


class Introduction(Scene):
    def construct(self):
        title = Text("Project presentation")
        subtitle = Text("An animation made with ManimGL").scale(0.6)
        subtitle.next_to(title, DOWN)

        self.play(Write(title))
        self.play(FadeIn(subtitle, shift=UP))
        self.wait()
```

A ManimGL scene is a class derived from `Scene`. Its `construct` method defines the objects and animations shown on screen. Common building blocks include:

- `Text(...)` and `Tex(...)` for labels and mathematical expressions
- `Circle()`, `Square()`, `Line()`, and other geometric objects
- `self.play(...)` to run animations
- `self.wait(...)` to pause the scene
- `self.add(...)` to display an object immediately

## The microCI presentation

`microCI.py` contains a roughly 60-second presentation split into these scenes:

1. `Title` — what microCI is
2. `Problem` — vendor lock-in
3. `HowItWorks` — YAML to Bash
4. `Benefits` — portability, auditability, and reproducibility
5. `Usage` — how to run a pipeline
6. `Closing` — the main takeaway

## Render and preview

The `Makefile` provides repeatable commands. First activate the environment:

```bash
source .venv/bin/activate
```

Check the setup and Python syntax:

```bash
make test
```

Show every scene at low quality in a window (press `Esc` after each scene to continue):

```bash
make preview
```

Preview one scene in a 960x540 window:

```bash
make preview-title
make preview-how
make preview-usage
```

Available individual targets are `preview-title`, `preview-problem`, `preview-how`, `preview-benefits`, `preview-usage`, and `preview-closing`. Individual previews remain open for inspection; press `Esc` to close them. Preview targets do not write MP4 files; use `make render` to create videos.

Render the complete presentation at the default quality:

```bash
make render
```

`make` is an alias for the full render. Remove generated media with `make clean`.

For direct ManimGL usage:

```bash
manimgl microCI.py Title -w -l
manimgl --help
```

## Presentation workflow

1. Edit or add a scene class in `microCI.py`.
2. Run `make test` to check syntax.
3. Use the matching `make preview-*` target while developing.
4. Review the generated video in `media/videos/`.
5. Run `make render` for the complete presentation.

## Troubleshooting

If installation reports that `pangocairo` or `portaudio.h` is missing, install the system dependencies listed above and run the pip command again. If `manimgl` is not found, activate the environment or invoke it directly:

```bash
.venv/bin/manimgl presentation.py Introduction
```
