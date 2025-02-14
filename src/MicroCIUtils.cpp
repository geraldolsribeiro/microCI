// MIT License
//
// Copyright (C) 2022-2025 Geraldo Luis da Silva Ribeiro
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
auto banner() -> string {
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
auto version() -> string { return fmt::format("v{}", microCI_version); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto sanitizeName(const string &name) -> string {
  auto ret = name;

  // Troca os caracteres acentuados por versão sem acento
  map<string, string> tr = {{"ç", "c"}, {"á", "a"}, {"ã", "a"}, {"ê", "e"}, {"ó", "o"}, {"õ", "o"}};
  for (auto const &[from, to] : tr) {
    size_t pos = 0;
    while ((pos = ret.find(from, pos)) != string::npos) {
      ret.replace(pos, from.length(), to);
      pos += to.length();
    }
  }

  // Converte tudo para minúsculas
  transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return tolower(c); });

  // Caracteres não permitidos são trocados por _
  const auto allowedChars = "abcdefghijklmnopqrstuvwxyz01234567890";
  size_t found = ret.find_first_not_of(allowedChars);
  while (found != string::npos) {
    ret[found] = '_';
    found = ret.find_first_not_of(allowedChars, found + 1);
  }
  return ret;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepRequiredValue(const YAML::Node &step, const string &var) -> string {
  if (!step[var]) {
    throw std::invalid_argument(fmt::format("Campo {} não encontrado no passo", var));
  }
  return step[var].as<string>();
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepOptionalValue(const YAML::Node &step, const string &var, const string &defaultValue) -> string {
  if (step[var]) {
    return step[var].as<string>();
  } else {
    return defaultValue;
  }
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepName(const YAML::Node &step) -> string { return stepRequiredValue(step, "name"); }

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
auto stepDescription(const YAML::Node &step, const string &defaultValue) -> string {
  return stepOptionalValue(step, "description", defaultValue);
  return defaultValue;
}

}  // namespace microci
