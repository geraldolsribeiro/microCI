# microCI — Windows (MSYS2/MinGW64) Build Report

> **Date:** 30–31 September 2026  
> **Author:** Geraldo Luis da Silva Ribeiro (with AI assistance)  
> **Result:** ✅ `microCI` now builds and runs natively on Windows via MSYS2 (MinGW64)

---

## 1. Objective

Port the microCI build system (previously Linux/macOS-only) to compile and link a working
Windows PE executable using the MSYS2 / MinGW-w64 GCC toolchain, without modifying the
upstream C++ source logic beyond portability guards.

### 1.1 Driving prompt

The single instruction that drove this task (summarized):

> **Get `microCI` to build and run on Windows.** I have **MSYS2 / MinGW64 installed**
> (at `C:\msys64`). Assume that all the needed libraries are already available.
> Make the build produce a working `microCI.exe`, verify it actually runs
> (version, help, and script generation), and add a Windows CI job — without changing the
> core C++ logic beyond portability guards.

The repository's `AGENTS.md` was created in a separate session with its own instruction:

> **Make the `AGENTS.md` file of this project**, which emulates the same as Claude Code does to
> fully describe the repo and provide a faster way for later usage with an AI agent.

At the end of this Windows build task, that `AGENTS.md` was updated to reflect the new
Windows/MinGW build target so the next agent starts from an accurate map.

Because the toolchain and every dependency were already present, the work was purely about
making the **existing** Linux/macOS build system and a few POSIX-only code paths portable to
MinGW — not about setting up an environment.

---

## 2. Environment

| Component | Version / Path |
|-----------|---------------|
| OS | Windows 10/11 (x86_64) |
| Shell | MSYS2 (MINGW64) |
| MSYS2 root | `C:\msys64` |
| GCC | `x86_64-w64-mingw32-g++ 16.2.0` |
| Linker | `x86_64-w64-mingw32-ld.exe` |
| C++ standard | C++20 (`-std=c++20`) |
| Key libraries | `libyaml-cpp`, `libcrypto` (OpenSSL), `libzstd`, `libz` (all MinGW-w64 DLL import libs) |
| Header-only deps | `argh`, `inja`, `nlohmann/json`, `inicpp` (in `include/3rd/`) |
| UPX | 3.96w (for binary compression) |
| `xxd` | provided by the `vim` MSYS2 package |

### MSYS2 packages required

> All of these were **already installed** in the working environment
> (`C:\msys64`) before this task began — nothing had to be set up.
> The list below is for reference / reproducibility.

```
make
vim                              # provides xxd (needed by asset-header generation)
mingw-w64-x86_64-gcc             # compiler + linker + strip
mingw-w64-x86_64-yaml-cpp        # YAML parsing
mingw-w64-x86_64-openssl         # crypto (linked via -lcrypto)
mingw-w64-x86_64-zstd            # compression
mingw-w64-x86_64-zlib            # zlib
mingw-w64-x86_64-upx             # binary compression
```

---

## 3. Initial Findings (Debugging Process)

### 3.1 First build attempt — link failure

The first `make -C src` on MinGW produced this error:

```
x86_64-w64-mingw32/bin/ld.exe: undefined reference to
  `std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>
   >::basic_string(std::__cxx11::basic_string<char, std::char_traits<char>,
   std::allocator<char> >&&)'
collect2.exe: error: ld returned 1 exit status
```

This is a **linker error**, not a compiler error — the symbol is referenced in the object
code but the linker cannot resolve it to a definition in the linked libraries.

### 3.2 Flag bisection

To isolate the cause, each Makefile flag was tested individually against a minimal
C++20 translation unit that exercises `std::string` move-construction:

```cpp
// microci_test_str.cpp
#include <string>
int main() {
    std::string a = "hello";
    std::string b = std::move(a);   // triggers the move constructor
    return b.size() == 5 ? 0 : 1;
}
```

| Flag(s) tested | Result |
|---------------|--------|
| `-fdata-sections` | ✅ PASS |
| `-ffunction-sections` | ✅ PASS |
| `-Wl,--gc-sections` | ✅ PASS |
| `-s` (strip) | ✅ PASS |
| **`-Os`** | ❌ **FAIL** — same undefined reference |
| `-O0` | ✅ PASS |
| `-O1` | ✅ PASS |
| `-O2` | ✅ PASS |
| `-O3` | ✅ PASS |
| **`-Oz`** | ❌ **FAIL** — same undefined reference |
| `-O2 -fdata-sections -ffunction-sections -Wl,--gc-sections -s` | ✅ PASS (10 752 bytes) |

**Conclusion:** On MinGW-w64 GCC 16.2.0, the size-optimization levels (`-Os`, `-Oz`)
mis-generate the reference to `std::string`'s move constructor. This is a known class of
MinGW-w64 libstdc++ issues where the compiler emits a reference to a symbol that the
import library (`libstdc++.dll.a`) does not export under that exact mangled name at those
optimization levels. Using `-O2` avoids the issue while still producing a small binary
(especially combined with section GC and UPX).

### 3.3 `.exe` extension mismatch

After fixing the link error, `make` failed at the `strip` step:

```
strip.exe: '../bin/microCI': No such file
```

The MinGW linker automatically appends `.exe` to the output filename. The Makefile
referenced `../bin/microCI` (no extension), so `strip` and `upx` couldn't find the file.

**Fix:** Introduced an `EXE_EXT` Makefile variable (empty on Linux/macOS, `.exe` on MinGW)
and substituted it into every target path.

### 3.4 `RANDOM_8` undefined on Windows

After the binary linked, running `microCI.exe --list` on the project's own `.microCI.yml`
produced:

```
[inja.exception.render_error] (at 6:47) variable 'RANDOM_8' not found
```

Investigation of `src/MicroCI.cpp` (`DefaultDataTemplate()`) revealed:

```cpp
#ifdef __APPLE__
  data["RANDOM_8"] = "$(uuidgen | head -c 8)";
#endif
#ifdef __linux__
  data["RANDOM_8"] = "$(head -c 8 /proc/sys/kernel/random/uuid)";
#endif
```

There was **no `_WIN32` branch**, so on Windows the variable was never set, and any inja
template referencing `{{ RANDOM_8 }}` (used in the base `PluginStepParser::prepareRunDocker`
and in Asciidoc, Beamer, Mermaid, Pandoc, MkdocsMaterial plugins) threw a render error.

**Fix:** Added a `_WIN32` branch using `/dev/urandom` (available in Git Bash / MSYS2):

```cpp
#ifdef _WIN32
  data["RANDOM_8"] = R"MC($(head -c 4 /dev/urandom | od -An -tx1 | tr -d ' \n'))MC";
#endif
```

### 3.5 POSIX-only system calls

`src/MicroCI.cpp` used `getpwuid(getuid())` and included `<pwd.h>`, `<sys/types.h>`,
`<unistd.h>` unconditionally. These are POSIX-only and unavailable on MinGW.

**Fix:** Guarded with `#ifdef _WIN32` / `#else`, using `std::getenv("USERPROFILE")` on
Windows to resolve the home directory.

### 3.6 `std::filesystem::path` implicit conversion

`src/main.cpp` line 963 relied on an implicit `std::filesystem::path → std::string`
conversion that MinGW's libstdc++ does not provide (or that `-Werror` rejects).

**Fix:** Added an explicit `.string()` call.

---

## 4. Changes Made

### 4.1 `src/Makefile`

| Change | Detail |
|--------|--------|
| `IS_MINGW` detection | `ifneq (,$(findstring MINGW,$(UNAME_S)))` → sets `IS_MINGW=1` |
| `EXE_EXT` variable | `.exe` on MinGW, empty otherwise; substituted in all target paths |
| `-static` skipped on MinGW | Static system libs reference Windows DLLs that `-static` prevents linking |
| `-Os` → `-O2` on MinGW | Avoids the size-opt move-ctor link bug; size flags still apply |
| Target paths | `../bin/microCI$(EXE_EXT)` in `all`, `diagram`, build rule, `clean` |

### 4.2 `src/MicroCI.cpp`

| Change | Detail |
|--------|--------|
| Conditional includes | `<cstdlib>` on `_WIN32`; `<pwd.h>`/`<sys/types.h>`/`<unistd.h>` only on POSIX |
| Home directory | `std::getenv("USERPROFILE")` on Windows; `getpwuid(getuid())` on POSIX |
| `RANDOM_8` | Added `_WIN32` branch using `/dev/urandom` |

### 4.3 `src/main.cpp`

| Change | Detail |
|--------|--------|
| Line 963 | Added `.string()` to `std::filesystem::path` → `std::string` conversion |

### 4.4 `.github/workflows/c-cpp.yml`

Added a `build-windows` job:

- Runner: `windows-latest`
- Shell: `msys2 {0}` (via `msys2/setup-msys2@v2`)
- Critical: `git config --global core.autocrlf input` **before** `actions/checkout`
  (otherwise Git converts LF→CRLF and `make` fails)
- Installs: `make`, `vim`, `mingw-w64-x86_64-{gcc,yaml-cpp,openssl,zstd,zlib,upx}`
- Builds: `make -C src clean all`
- Verifies: `bin/microCI.exe --version`
- Uploads: `bin/microCI.exe` as artifact `microCI-windows`

---

## 5. Build & Verification Results

### 5.1 Build output

```
=== BUILD START Thu Oct  1 00:27:23     2026 ===
Single compilation unit was generated
...
        File size         Ratio      Format      Name
   --------------------   ------   -----------   -----------
   1473536 ->    311808   21.16%    win64/pe     microCI.exe

Packed 1 file.
=== MAKE_EXIT=0 ===
```

- **Input size:** 1 473 536 bytes (1.47 MB)
- **Output size:** 311 808 bytes (311 KB) — **78.8% reduction** via UPX

### 5.2 Runtime verification

| Test | Result |
|------|--------|
| `microCI.exe --version` | `v0.50.0` (exit 0) ✅ |
| `microCI.exe --help` | Full banner + options (exit 0) ✅ |
| `microCI.exe -i .microCI.yml --list` | 15 steps listed (exit 0) ✅ |
| Script generation (bash plugin) | 454-line valid Bash script ✅ |
| `bash -n` syntax check on generated script | SYNTAX OK ✅ |
| `RANDOM_8` substitution | `$(head -c 4 /dev/urandom \| od -An -tx1 \| tr -d ' \n')` ✅ |
| `docker run` line present | ✅ |
| ERROR count in generated script | 0 ✅ |

---

## 6. Gotchas & Recommendations

| # | Gotcha | Mitigation |
|---|--------|-----------|
| 1 | **`-Os`/`-Oz` break `std::string` linking on MinGW** | Use `-O2` on MinGW; revisit if a future GCC release fixes it |
| 2 | **MinGW linker appends `.exe`** | Use `$(EXE_EXT)` in all Makefile target paths |
| 3 | **Git CRLF conversion breaks `make`** | Set `core.autocrlf input` before `actions/checkout` in CI |
| 4 | **`xxd` is in the `vim` package** (not a standalone package) | Install `vim` in MSYS2 for `xxd` |
| 5 | **`-static` prevents linking Windows DLLs** | Skip `-static` on MinGW; link dynamically against import libs |
| 6 | **`/dev/urandom` works in MSYS2/Git Bash** | Safe to use in generated scripts for `RANDOM_8` |
| 7 | **`getpwuid`/`<pwd.h>` are POSIX-only** | Guard with `#ifdef _WIN32`; use `USERPROFILE` env var |
| 8 | **`std::filesystem::path` → `std::string` needs explicit `.string()` on MinGW** | Add `.string()` where the conversion is needed |
| 9 | **Regenerated asset headers get CRLF on Windows** | Revert `include/new|sh|help|external/*.hpp` after local Windows builds; CI on Linux regenerates them with LF |

---

## 7. Reproduction Steps (from scratch on a clean Windows machine)

```powershell
# 1. Install MSYS2 from https://www.msys2.org/
# 2. Open the MSYS2 MINGW64 shell and:
pacman -Syu
pacman -S --needed make vim mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-yaml-cpp mingw-w64-x86_64-openssl \
    mingw-w64-x86_64-zstd mingw-w64-x86_64-zlib mingw-w64-x86_64-upx

# 3. Clone the repo (with LF line endings!)
git config --global core.autocrlf input
git clone <repo-url> microCI
cd microCI

# 4. Download header-only 3rd-party libs (if not already in include/3rd/)
mkdir -p include/3rd/nlohmann
wget -P include/3rd https://raw.githubusercontent.com/adishavit/argh/master/argh.h
wget -P include/3rd https://raw.githubusercontent.com/pantor/inja/master/single_include/inja/inja.hpp
wget -P include/3rd https://raw.githubusercontent.com/Rookfighter/inifile-cpp/master/include/inicpp.h
wget -P include/3rd/nlohmann https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

# 5. Build
make -C src clean all

# 6. Verify
bin/microCI.exe --version
bin/microCI.exe --help
```

---

## 8. Files Changed (final diff)

```
.github/workflows/c-cpp.yml | 49 +++++++++++++++++++++++++++++++++++++++++++++
src/Makefile                | 36 ++++++++++++++++++++++++++++-----
src/MicroCI.cpp             | 16 +++++++++++++++
src/main.cpp                |  2 +-
4 files changed, 97 insertions(+), 6 deletions(-)
```

---

*End of report.*