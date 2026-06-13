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

#include <string>

namespace microci {

using std::string;

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto banner() -> std::string {
  return fmt::format(R"(


                           ░░░░░░░░░░░░░░░░░
                           ░░░░░░░█▀▀░▀█▀░░░
                           ░░░█░█░█░░░░█░░░░
                           ░░░█▀▀░▀▀▀░▀▀▀░░░
                           ░░░▀░░░░░░░░░░░░░
                           ░░░░░░░░░░░░░░░░░
                             microCI {}

)",
                     microCI_version);
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto version() -> std::string { return fmt::format("v{}", microCI_version); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto sanitizeName(const std::string &name) -> std::string {
  auto ret = name;

  // Replace accented characters with unaccented versions
  std::map<string, std::string> tr = {{"ç", "c"}, {"á", "a"}, {"ã", "a"}, {"ê", "e"}, {"ó", "o"}, {"õ", "o"}};
  for (auto const &[from, to] : tr) {
    size_t pos = 0;
    while ((pos = ret.find(from, pos)) != std::string::npos) {
      ret.replace(pos, from.length(), to);
      pos += to.length();
    }
  }

  // Convert everything to lowercase
  transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) -> int { return tolower(c); });

  // Characters not allowed are replaced by _
  const auto allowedChars = "abcdefghijklmnopqrstuvwxyz01234567890";
  size_t found            = ret.find_first_not_of(allowedChars);
  while (found != std::string::npos) {
    ret[found] = '_';
    found      = ret.find_first_not_of(allowedChars, found + 1);
  }
  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepRequiredValue(const YAML::Node &step, const std::string &var) -> std::string {
  if (!step[var]) {
    throw std::invalid_argument(fmt::format("Field {} not found in step", var));
  }
  return step[var].as<std::string>();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepOptionalValue(const YAML::Node &step, const std::string &var, const std::string &defaultValue) -> std::string {
  if (step[var]) {
    return step[var].as<std::string>();
  } else {
    return defaultValue;
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepName(const YAML::Node &step) -> std::string { return stepRequiredValue(step, "name"); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepDescription(const YAML::Node &step, const std::string &defaultValue) -> std::string {
  return stepOptionalValue(step, "description", defaultValue);
  return defaultValue;
}

}  // namespace microci
