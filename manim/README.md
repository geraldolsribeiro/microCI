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

## Render an animation

With the virtual environment active, render a scene using:

```bash
manimgl presentation.py Introduction
```

For a faster preview, use low quality:

```bash
manimgl presentation.py Introduction -w
```

The rendered video is written under `media/videos/`. Open the generated file to review the result.

Useful options can be listed with:

```bash
manimgl --help
```

## Presentation workflow

1. Add one scene class per presentation section.
2. Preview scenes at low quality while developing.
3. Keep text and visuals readable at the target resolution.
4. Render the final scenes at the desired quality.
5. Review the generated videos in `media/videos/` before assembling the presentation.

Example with multiple scenes:

```bash
manimgl presentation.py Introduction
manimgl presentation.py Results
manimgl presentation.py Conclusion
```

## Troubleshooting

If installation reports that `pangocairo` or `portaudio.h` is missing, install the system dependencies listed above and run the pip command again. If `manimgl` is not found, activate the environment or invoke it directly:

```bash
.venv/bin/manimgl presentation.py Introduction
```
