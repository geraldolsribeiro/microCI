#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

#if defined(__clang__)
/* Clang compiler detected */
// Note: Clang also defines __GNUC__, so this check MUST come first.
#define COMPILER_NAME "Clang " STRINGIFY(__clang_major__) "." STRINGIFY(__clang_minor__)

#elif defined(__GNUC__) || defined(__GNUG__)
/* GCC compiler detected */
// This block runs ONLY if it is pure GCC (and not Clang).
#define COMPILER_NAME "GCC " STRINGIFY(__GNUC__) "." STRINGIFY(__GNUC_MINOR__)

#else
#define COMPILER_NAME "Unknown Compiler"
#endif

#include <iostream>

int main() {
  std::cout << "Hello from C++ compiled by " << COMPILER_NAME << std::endl;
  return 0;
}
