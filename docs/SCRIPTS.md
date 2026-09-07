# Scripts

Run these commands in PowerShell from the project directory. Build scripts need
the Visual Studio C++ workload and CMake tools described in the
[README](../README.md). Python 3 is needed to regenerate textures; formatting
requires Python 3.11+ with pip.

## Build and check

```powershell
# Build x64 Release and run the 30 behavior checks.
.\scripts\build.ps1 -Configuration Release -Platform x64 -Test

# Build x64 Release, then check behavior, rendering, animation and window closure.
.\scripts\verify.ps1

# Repeat verification using an existing x64 Release build.
.\scripts\verify.ps1 -SkipBuild

# Also replace the README screenshot with a fresh overview capture.
.\scripts\verify.ps1 -SkipBuild -RefreshScreenshots

# Test closing the native window, using an existing matching build.
.\scripts\test-shutdown.ps1 -Configuration Release -Platform x64 -Iterations 5
```

`build.ps1` defaults to Release/x64 and accepts Debug/Release and x64/Win32.
`test-shutdown.ps1` defaults to Debug/Win32 with five runs; it checks the
title-bar close action. `verify.ps1` always uses Release/x64, opens temporary
farm windows, saves framebuffer captures and logs under ignored
`build/verification/`, and checks exit status and OpenGL errors. Captures need
visual inspection to assess appearance.

## Maintenance

```powershell
# Download/build FreeGLUT only; Visual Studio already runs this automatically.
.\scripts\setup.ps1 -Configuration Release -Platform x64

# Update Visual Studio source entries and folders after adding/removing C++ files.
.\scripts\sync-project.ps1

# Regenerate the four bundled BMP textures, overwriting the existing files.
python .\scripts\generate-textures.py
```

`setup.ps1` defaults to Debug/x64. FreeGLUT is downloaded and its checksum
verified on the first build of each build directory. Visual Studio caches it
under `build/deps/<platform>/_deps/`; the documented CMake build uses
`build/cmake/_deps/`. Later builds reuse those files without downloading again.
Clearing the cache requires internet access on the next build. Keep the
generated `FREEGLUT-LICENSE.txt` when sharing an executable.

The project and texture generation scripts modify project files and assets; they
are optional when simply building or running the farm.

## Formatting

```powershell
# Format all project code, Markdown, scripts, CMake, XML and text configuration.
.\scripts\format.ps1

# Check formatting without changing project files; exits nonzero on differences.
.\scripts\format.ps1 -Check

# Format Markdown only (add -Check to check Markdown only).
.\scripts\format.ps1 -Only Markdown
```

Run from any directory by using the script's full path. The first run downloads
pinned tools into ignored `build/format/`; later runs reuse them. No global
packages are installed. Python must be on `PATH`; clang-format is found on
`PATH` or in Visual Studio's C++ tools. Markdown-only runs do not need
clang-format.

The command uses clang-format for C++, mdformat with GitHub tables for Markdown,
PSScriptAnalyzer for PowerShell, Ruff for Python, cmake-format for CMake, and
the .NET XML writer for Visual Studio files. Other project configuration files
receive UTF-8/LF and whitespace normalization. It covers root text files and
`src/`, `docs/`, `scripts/`, and `cmake/`; assets, screenshots, dependencies,
build outputs, and IDE settings are excluded.

Visual Studio reads `.editorconfig` automatically when **Follow project coding
conventions** is enabled. It controls indentation, encoding and whitespace for
supported file types; it does not reformat all existing files. Reopen code files
after changing it. Other editors may need an EditorConfig extension, and Visual
Studio's XML editor does not apply it. See
[EditorConfig support](https://learn.microsoft.com/en-us/visualstudio/ide/create-portable-custom-editor-options?view=visualstudio).

For C++, keep **Enable ClangFormat support** enabled in Visual Studio's C/C++
formatting options. The included `.clang-format` defines the code layout. Use
**Ctrl+K, Ctrl+D** to format the current document. See
[C++ formatting](https://learn.microsoft.com/en-us/visualstudio/ide/reference/options-text-editor-c-cpp-formatting?view=visualstudio).
