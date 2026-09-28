from manimlib import *


class Title(Scene):
    """Opening: microCI in one sentence (about 8 seconds)."""

    def construct(self):
        title = Text("microCI", font_size=72, color=BLUE)
        tagline = Text("Write your pipeline once. Run it anywhere.", font_size=30)
        tagline.next_to(title, DOWN, buff=0.35)
        self.play(Write(title), run_time=1.5)
        self.play(FadeIn(tagline, shift=UP), run_time=1)
        self.wait(5.5)


class Problem(Scene):
    """The vendor lock-in problem (about 10 seconds)."""

    def construct(self):
        heading = Text("CI should not lock your project in", font_size=40)
        heading.to_edge(UP)
        vendors = VGroup(
            Text("GitHub Actions", font_size=30),
            Text("GitLab CI", font_size=30),
            Text("Jenkins", font_size=30),
        ).arrange(DOWN, buff=0.35)
        box = SurroundingRectangle(vendors, color=RED, buff=0.35)
        lock = Text("vendor", font_size=26, color=RED).next_to(box, RIGHT, buff=0.45)
        self.play(Write(heading))
        self.play(LaggedStart(*[FadeIn(item, shift=RIGHT) for item in vendors], lag_ratio=0.25))
        self.play(ShowCreation(box), Write(lock))
        self.wait(5)


class HowItWorks(Scene):
    """YAML to Bash to execution (about 12 seconds)."""

    def construct(self):
        heading = Text("One definition. A plain Bash script.", font_size=38).to_edge(UP)
        yaml = VGroup(
            Text("pipeline.yml", font_size=28, color=YELLOW),
            Text("steps:", font_size=24),
            Text("  - build", font_size=24),
            Text("  - test", font_size=24),
        ).arrange(DOWN, aligned_edge=LEFT, buff=0.12)
        yaml_box = SurroundingRectangle(yaml, color=YELLOW, buff=0.3)
        bash = VGroup(
            Text("bash", font_size=28, color=GREEN),
            Text("./build", font_size=24),
            Text("./test", font_size=24),
        ).arrange(DOWN, aligned_edge=LEFT, buff=0.18)
        bash_box = SurroundingRectangle(bash, color=GREEN, buff=0.3)
        arrow = Arrow(yaml_box.get_right(), bash_box.get_left(), buff=0.35)
        anywhere = Text("run anywhere", font_size=28, color=BLUE).next_to(bash_box, DOWN, buff=0.5)
        bash_box.shift(RIGHT * 2.3)
        arrow.put_start_and_end_on(yaml_box.get_right(), bash_box.get_left())
        self.play(Write(heading))
        self.play(ShowCreation(yaml_box), Write(yaml))
        self.play(GrowArrow(arrow), ShowCreation(bash_box), Write(bash))
        self.play(FadeIn(anywhere, shift=UP))
        self.wait(5)


class Benefits(Scene):
    """Benefits (about 10 seconds)."""

    def construct(self):
        heading = Text("Benefits", font_size=46).to_edge(UP)
        items = VGroup(
            Text("Portable", font_size=32, color=BLUE),
            Text("Auditable", font_size=32, color=GREEN),
            Text("Reproducible", font_size=32, color=YELLOW),
            Text("Vendor independent", font_size=32, color=PURPLE),
        ).arrange(DOWN, buff=0.28)
        self.play(Write(heading))
        self.play(LaggedStart(*[FadeIn(item, shift=RIGHT) for item in items], lag_ratio=0.2))
        self.wait(6)


class Usage(Scene):
    """Getting started (about 10 seconds)."""

    def construct(self):
        heading = Text("Getting started", font_size=44).to_edge(UP)
        commands = VGroup(
            Text("1. Write your pipeline.yml", font_size=30),
            Text("2. microCI | bash", font_size=30, color=GREEN),
            Text("3. Review and run the generated script", font_size=30),
        ).arrange(DOWN, aligned_edge=LEFT, buff=0.38)
        self.play(Write(heading))
        for command in commands:
            self.play(FadeIn(command, shift=RIGHT), run_time=0.8)
        self.wait(5)


class Closing(Scene):
    """Closing message (about 10 seconds)."""

    def construct(self):
        title = Text("microCI", font_size=64, color=BLUE)
        message = Text("One source of truth. Everywhere.", font_size=36)
        message.next_to(title, DOWN, buff=0.4)
        self.play(Write(title))
        self.play(FadeIn(message, shift=UP))
        self.wait(7)
