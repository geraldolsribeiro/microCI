// MIT License
//
// Copyright (C) 2022-2026 Geraldo Luis da Silva Ribeiro
//
// ░░░░░░░░░░░░░░░░░
// ░░░░░░░█▀▀░▀█▀░░░
// ░░░█░█░█░░░░█░░░░
// ░░░█▀▀░▀▀▀░▀▀▀░░░
// ░░░▀░░░░░░░░░░░░░
// ░░░░░░░░░░░░░░░░░
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <algorithm>

namespace {

constexpr char kHorizontal[]  = "─";
constexpr char kTopLeft[]     = "┌";
constexpr char kTopRight[]    = "┐";
constexpr char kBottomLeft[]  = "└";
constexpr char kBottomRight[] = "┘";
constexpr char kMiddleLeft[]  = "├";
constexpr char kMiddleRight[] = "┤";
constexpr char kVertical[]    = "│";

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void draw_border(std::ostream &os, const std::string &left, const std::string &right, std::size_t width) {
  os << "echo \"" << left;
  for (std::size_t i = 0; i < width; ++i) {
    os << kHorizontal;
  }
  os << right << "\"\n";
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
std::size_t display_width(const std::string &title, const std::vector<std::string> &lines) {
  const auto line_width =
      std::max_element(lines.begin(), lines.end(),
                       [](const std::string &lhs, const std::string &rhs) { return lhs.size() < rhs.size(); });
  const auto max_line = line_width == lines.end() ? std::string{} : *line_width;
  return std::max(title.size(), max_line.size());
}

}  // namespace

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void drawConsoleBox(std::ostream &os, const std::string &title, const std::vector<std::string> &lines) {
  const std::size_t inner_width = display_width(title, lines) + 4;

  draw_border(os, kTopLeft, kTopRight, inner_width);

  const std::size_t title_padding_left  = (inner_width - title.size()) / 2;
  const std::size_t title_padding_right = inner_width - title.size() - title_padding_left;
  os << "echo \"" << kVertical << std::string(title_padding_left, ' ') << title << std::string(title_padding_right, ' ')
     << kVertical << "\"\n";

  draw_border(os, kMiddleLeft, kMiddleRight, inner_width);

  for (const auto &line : lines) {
    const std::size_t padding = inner_width - line.size();
    os << "echo \"" << kVertical << ' ' << line << std::string(padding - 1, ' ') << kVertical << "\"\n";
  }

  draw_border(os, kBottomLeft, kBottomRight, inner_width);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void drawConsoleBox(const std::string &title, const std::vector<std::string> &lines) {
  drawConsoleBox(std::cout, title, lines);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void errorConsoleBox(const std::vector<std::string> &lines) { drawConsoleBox("ERROR", lines); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void criticalErrorConsoleBox(const std::vector<std::string> &lines) {
  drawConsoleBox("ERROR", lines);
  std::cout << "# *** End of Execution ***\n";
  std::cout << "exit 1" << std::endl;
  std::terminate();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void infoConsoleBox(const std::vector<std::string> &lines) { drawConsoleBox("INFORMATION", lines); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
void debugConsoleBox(const std::vector<std::string> &lines) { drawConsoleBox("DEBUG", lines); }
