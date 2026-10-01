# AI Task Log

## Summary

| Task | Status | Branch |
|------|--------|--------|
| Fully static MinGW Windows build (zero DLLs) | done | fix/ai-static-linking-mingw |

## Tasks

### Fully static MinGW Windows build (zero DLLs)

- **Status:** done
- **Branch:** fix/ai-static-linking-mingw
- **Started:** 2026-10-01 20:54:46
- **Completed:** 2026-10-01 21:45:00
- **Time Spent:** ~50 min

#### Final Result

**Single `microCI.exe` (7.8 MB) with ZERO third-party DLL dependencies.**
Only depends on Windows system DLLs (KERNEL32, msvcrt, CRYPT32, ADVAPI32, USER32, WS2_32)
which are always present on any Windows x64 machine.

```
bin/
└── microCI.exe   (7.8 MB — fully self-contained)
```

#### Root Cause Analysis

The `__imp_` prefix issue was NOT a fundamental MinGW limitation (as initially thought).
It was caused by **header-level `__declspec(dllimport)` annotations** that tell the compiler
to generate DLL-import-style references:

| Library | Header | Macro | Fix |
|---------|--------|-------|-----|
| yaml-cpp | `dll.h` | `YAML_CPP_API` → `__declspec(dllimport)` | `-DYAML_CPP_STATIC_DEFINE` |
| winpthread | `pthread_compat.h` | `WINPTHREAD_API` → `__declspec(dllimport)` | `-DWINPTHREAD_STATIC` |

With these defines, the compiler generates **normal function references** (no `__imp_` prefix),
which the static `.a` libraries satisfy directly.

The remaining issue was **link order**: `libstdc++.a` (added by GCC driver) references
`pthread_cond_broadcast` from `libwinpthread.a`, but appears AFTER it in the link command.
Fixed with `-Wl,--start-group` / `--end-group` + explicitly including `libstdc++.a` and
`libgcc.a` in the group.

#### Changes to `src/Makefile`

1. **MinGW detection fix**: Added `g++ -dumpmachine` check for `mingw32` (fallback when
   `uname -s` returns `WindowsNT` instead of `MINGW64`).
2. **`-DYAML_CPP_STATIC_DEFINE`**: Prevents `__declspec(dllimport)` in yaml-cpp headers.
3. **`-DWINPTHREAD_STATIC`**: Prevents `__declspec(dllimport)` in pthread headers.
4. **Static library linking**: `-l:libyaml-cpp.a -l:libcrypto.a -l:libzstd.a -l:libz.a
   -l:libwinpthread.a -l:libstdc++.a -l:libgcc.a`
5. **`-Wl,--start-group` / `--end-group`**: Resolves circular references between static libs.
6. **UPX optional**: Skip compression if UPX not in PATH.
7. **Removed DLL bundling**: No longer needed (zero third-party DLLs).

#### Subtasks

- [x] Diagnose `__imp_` link errors (root cause: `__declspec(dllimport)` in headers)
- [x] Fix MinGW detection in Makefile
- [x] Add `-DYAML_CPP_STATIC_DEFINE` (yaml-cpp static linking)
- [x] Add `-DWINPTHREAD_STATIC` (winpthread static linking)
- [x] Add `--start-group`/`--end-group` + explicit libstdc++/libgcc in group
- [x] Make UPX optional
- [x] Remove DLL bundling step
- [x] Rebuild successfully (EXIT_CODE=0)
- [x] Verify: only Windows system DLLs in import table
- [x] Test under native PowerShell (clean PATH): --version, --list, --help all pass
- [x] Commit and push

#### Build Command

```powershell
& "D:\SDK_ARM\msys64\usr\bin\bash.exe" -c "export PATH=/mingw64/bin:/usr/bin:/bin; cd /d/Repos/microCI && make -C src clean all 2>&1"
```

#### Verification

```powershell
# DLL deps (should be ONLY Windows system DLLs):
objdump -p bin/microCI.exe | grep "DLL Name"
# → ADVAPI32.dll, CRYPT32.dll, KERNEL32.dll, msvcrt.dll, USER32.dll, WS2_32.dll

# Native test (clean PATH, no MSYS2):
$env:PATH = "C:\Windows\System32;C:\Windows"
D:\Repos\microCI\bin\microCI.exe --version   # → v0.50.0
D:\Repos\microCI\bin\microCI.exe --list      # → 15 steps
```
