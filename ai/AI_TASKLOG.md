# AI Task Log

## Summary

| Task | Status | Branch |
|------|--------|--------|
| Full static linking for MinGW Windows build | in_progress | fix/ai-static-linking-mingw |

## Tasks

### Full static linking for MinGW Windows build

- **Status:** in_progress
- **Branch:** fix/ai-static-linking-mingw
- **Started:** 2026-10-01 20:54:46
- **Time Spent:** (in progress)

#### Findings & Notes

- The MinGW build currently links dynamically against `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libyaml-cpp.dll`.
- All static `.a` libraries are available in `D:\SDK_ARM\msys64\mingw64\lib\`:
  - `libyaml-cpp.a` (1.0 MB), `libstdc++.a` (8.3 MB), `libgcc_s.a` (82 KB)
  - `libcrypto.a` (9.7 MB), `libzstd.a` (1.3 MB), `libz.a` (115 KB)
- The Makefile skips `-static` on MinGW because static OpenSSL references Windows system DLLs.
- Fix: use `-static` unconditionally, and add `-lws2_32 -lcrypt32 -ladvapi32 -lgdi32 -luser32` for MinGW.
- MSYS2 is at `D:\SDK_ARM\msys64` (MINGW64 shell).

#### Subtasks

- [x] Create AI_TASKLOG.md
- [x] Create branch `fix/ai-static-linking-mingw`
- [ ] Edit `src/Makefile` — enable `-static` on MinGW + add Windows system libs
- [ ] Rebuild with `make -C src clean all` (from MSYS2 MINGW64 shell)
- [ ] Verify no DLL dependencies (objdump or dumpbin)
- [ ] Test under MSYS2 bash (version, help, list, script generation)
- [ ] Test under native Windows PowerShell (same commands)
- [ ] Commit changes

#### Full Context Notes for AI Agents

- **Repo:** `D:\Repos\microCI`
- **MSYS2 path:** `D:\SDK_ARM\msys64` (MINGW64: `D:\SDK_ARM\msys64\mingw64`)
- **Build command:** From MSYS2 MINGW64 shell: `cd /d/Repos/microCI && make -C src clean all`
- **Binary output:** `bin/microCI.exe`
- **Key Makefile section to edit:** Lines 99-108 in `src/Makefile` (Section 3: Static Library Linking)
- **Current problematic code:**
  ```makefile
  ifneq ($(IS_MINGW),1)
  CXXFLAGS+= -static
  endif
  ```
- **Replacement:**
  ```makefile
  CXXFLAGS+= -static
  ifneq ($(IS_MINGW),1)
  else
  LDLIBS+= -lws2_32 -lcrypt32 -ladvapi32 -lgdi32 -luser32
  endif
  ```
- **LDLIBS already defined (line 149-152):** `-lyaml-cpp -lcrypto -lzstd -lz`
- **To verify no DLLs:** `objdump -p bin/microCI.exe | grep "DLL Name"` (should show only KERNEL32.dll and/or ntdll.dll)
- **To test from PowerShell:** `D:\Repos\microCI\bin\microCI.exe --version`
- **To test from MSYS2:** `bin/microCI.exe --version` (from repo root in MINGW64 shell)
- **UPX is applied automatically** by the Makefile after linking.
- **Git identity:** `GIT_AUTHOR_NAME="AI_bot"`, `GIT_AUTHOR_EMAIL="ai_bot@intmain.io"`, same for committer.
