# AI Task Log

## Summary

| Task | Status | Branch |
|------|--------|--------|
| Portable MinGW Windows build (zero missing DLLs) | done | fix/ai-static-linking-mingw |

## Tasks

### Portable MinGW Windows build (zero missing DLLs)

- **Status:** done
- **Branch:** fix/ai-static-linking-mingw
- **Started:** 2026-10-01 20:54:46
- **Completed:** 2026-10-01 21:20:00
- **Time Spent:** ~25 min

#### Findings & Notes

- **Root cause of build failure:** The Makefile's MinGW detection used `uname -s` which returns
  `WindowsNT` (not `MINGW64`) when invoked from PowerShell. This caused the build to use
  `-static` + `-Os` (Linux flags) instead of the MinGW-specific flags, leading to link errors.
- **Root cause of `__imp_` link errors:** MinGW GCC generates `__imp_` prefixed references for
  ALL external functions (DLL import style). The static `.a` libraries don't have these symbols —
  only the `.dll.a` import libraries do. The MinGW linker defaults to picking `.a` over `.dll.a`,
  so `-Wl,-Bstatic` or `-static` both fail.
- **Solution:** Use `-l:lib<name>.dll.a` syntax to explicitly force the import library. This
  resolves the `__imp_` symbols while the actual code is embedded via the static library.
- **`-static-libgcc -static-libstdc++`** eliminates `libgcc_s_seh-1.dll` and `libstdc++-6.dll`
  from the main binary's dependencies. However, `libyaml-cpp.dll` still needs them (transitive dep).
- **Final result:** Binary + 5 bundled DLLs in `bin/` = portable folder that runs on any Windows
  machine with zero external dependencies beyond Windows system DLLs.

#### What was changed in `src/Makefile`

1. **MinGW detection fix** (line ~75): Added `g++ -dumpmachine` check for `mingw32` as a
   fallback when `uname -s` doesn't return `MINGW64`.
2. **Static runtime linking** (line ~110): Added `-static-libgcc -static-libstdc++` for MinGW
   to eliminate libgcc/libstdc++ DLL deps from the main binary.
3. **Import library linking** (line ~155): Use `-l:lib<name>.dll.a` syntax for MinGW to force
   the linker to use import libraries (resolves `__imp_` symbols).
4. **Windows system DLLs** (line ~167): Added `-lcrypt32 -lws2_32 -ladvapi32 -lgdi32 -luser32`
   for OpenSSL's Windows API references.
5. **UPX optional** (line ~370): Made UPX compression optional (skip if not in PATH).
6. **DLL bundling** (line ~371): Auto-copy required DLLs to `bin/` after build.

#### Bundled DLLs (in `bin/`)

| DLL | Size | Purpose |
|-----|------|---------|
| `libcrypto-3-x64.dll` | 5.5 MB | OpenSSL crypto |
| `libgcc_s_seh-1.dll` | 149 KB | GCC runtime (needed by libyaml-cpp.dll) |
| `libstdc++-6.dll` | 2.6 MB | C++ stdlib (needed by libyaml-cpp.dll) |
| `libwinpthread-1.dll` | 66 KB | POSIX threads |
| `libyaml-cpp.dll` | 535 KB | YAML parsing |

Total bundle: ~9.3 MB (exe is 3 MB, DLLs are 6.3 MB)

#### Subtasks

- [x] Create AI_TASKLOG.md
- [x] Create branch `fix/ai-static-linking-mingw`
- [x] Diagnose build failure (MinGW detection + `__imp_` prefix issue)
- [x] Fix MinGW detection in Makefile (`g++ -dumpmachine` fallback)
- [x] Fix library linking (`-l:lib<name>.dll.a` syntax)
- [x] Add `-static-libgcc -static-libstdc++` for MinGW
- [x] Make UPX optional
- [x] Add DLL bundling step
- [x] Rebuild successfully
- [x] Verify DLL dependencies (only Windows system DLLs + bundled DLLs)
- [x] Test under MSYS2 bash (version, help, list)
- [x] Test under native Windows PowerShell (clean PATH)
- [x] Commit changes

#### Full Context Notes for AI Agents

- **Repo:** `D:\Repos\microCI`
- **MSYS2 path:** `D:\SDK_ARM\msys64` (MINGW64: `D:\SDK_ARM\msys64\mingw64`)
- **Build command:** `& "D:\SDK_ARM\msys64\usr\bin\bash.exe" -c "export PATH=/mingw64/bin:/usr/bin:/bin; cd /d/Repos/microCI && make -C src clean all 2>&1"`
- **Binary output:** `bin/microCI.exe` + 5 DLLs in `bin/`
- **To verify DLLs:** `objdump -p bin/microCI.exe | grep "DLL Name"`
- **To test from PowerShell (clean PATH):**
  ```powershell
  $oldPath = $env:PATH; $env:PATH = "C:\Windows\System32;C:\Windows"
  D:\Repos\microCI\bin\microCI.exe --version
  $env:PATH = $oldPath
  ```
- **Git identity:** `GIT_AUTHOR_NAME="AI_bot"`, `GIT_AUTHOR_EMAIL="ai_bot@intmain.io"`, same for committer.
- **Why not fully static?** MinGW GCC generates `__imp_` prefixed references for all external
  functions. Static `.a` libs don't have these symbols. Only `.dll.a` import libs do. This is a
  fundamental limitation of the MinGW toolchain. True single-file static builds require MSVC.
