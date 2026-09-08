# Scripts

Run these commands in PowerShell from the project directory. Build scripts need
the Visual Studio C++ workload and CMake tools described in the
[README](../README.md). Python 3 is needed to regenerate textures; formatting
requires Python 3.11+ with pip.

## Build and check

```powershell
# Build x64 Release and run the behavior checks.
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

To capture the barn routine directly from an existing Release/x64 build:

```powershell
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/barn.bmp --view barn --night --time 45
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/morning.bmp --view barn --morning --time 4
```

Capture the farmer and the online CC0 face texture:

```powershell
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/farmer.bmp --view farmer --time 4
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/face.bmp --view farmer-face --time 4 --no-hud
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/patrol.bmp --view farmer --night --time 80
.\build\x64\Release\csc3081_farm_project.exe --capture build/verification/farmer-morning.bmp --view farmer --morning --time 80
```

`farmer` follows the character after the requested time advance; `farmer-face`
frames the head for texture inspection. Verification includes the farmer in
daylight, wheat rows, night, wireframe, small windows and without textures. The
behavior suite checks repeated circuits, stalk clearance, inspection pauses,
frame-rate independence, pause/resume, camera controls, all four sides of the
night patrol and continuous day/night transfers through paths and gates.

The farmer update was verified with **83 behavior checks, 36 render cases, five
native window-close runs**, missing-assets startup and a 180-frame benchmark on
Windows Release/x64. Frame captures still need a visual review; a successful
render alone does not establish that a pose looks correct.

`--time` advances the simulation by that many seconds. `--morning` first
advances 60 seconds of night, then switches the herd and farmer to their morning
routines and applies the requested daytime advance. The `--view` presets include
`front`, `back`, `left`, `right`, `barn-left`, and `barn-right`. Add `--no-hud`
to capture the scene without on-screen help. Wind and structural study stay in
their right-hand overlays in every view, including small windows. The scene
always fills the window behind the overlays. `--no-study` captures the wind
panel with the study switched off.

## Maintenance

```powershell
# Re-download the checksum-verified CC0 head texture and convert it to BMP.
.\scripts\import-farmer-texture.ps1

# Download/build FreeGLUT only; Visual Studio already runs this automatically.
.\scripts\setup.ps1 -Configuration Release -Platform x64

# Update Visual Studio source entries and folders after adding/removing C++ files.
.\scripts\sync-project.ps1

# Regenerate the four bundled BMP textures, overwriting the existing files.
python .\scripts\generate-textures.py

# Optionally regenerate the bundled handwriting atlas and metrics.
python -m venv build/font/python
.\build\font\python\Scripts\python.exe -m pip install Pillow==12.1.1
.\build\font\python\Scripts\python.exe scripts/generate-font.py
.\scripts\format.ps1
```

`setup.ps1` defaults to Debug/x64. FreeGLUT is downloaded and its checksum
verified on the first build of each build directory. Visual Studio caches it
under `build/deps/<platform>/_deps/`; the documented CMake build uses
`build/cmake/_deps/`. Later builds reuse those files without downloading again.
Clearing the cache requires internet access on the next build. Keep the
generated `FREEGLUT-LICENSE.txt` and the complete `assets/` folder beside the
executable when sharing a build. Source and license notices are documented in
[Third-party resources](THIRD_PARTY.md).

The project and texture generation scripts modify project files and assets; they
are optional when simply building or running the farm. Font generation downloads
a pinned, checksum-verified font into `build/font/` on its first run. The
generated atlas, metrics and license are already included.

Farmer texture import uses Windows System.Drawing to convert TheNess's online
PNG to a 24-bit BMP without changing its size or contents. Face UVs are defined
in `src/characters/Farmer.cpp`. The BMP, source credit and CC0 license are
already bundled, so the import script is optional. No personal photo is used.

The shared labels are in `assets/text.txt`. Save edits and press **F9** while
running; no generation script is needed. Both build paths also embed the file as
a fallback for missing assets or omitted keys. Generated text headers stay under
`build/`, so the editable `.txt` file is the only source to maintain.

## Rebuild the handbook PDF

Install a LaTeX distribution with `pdflatex` (such as MiKTeX or TeX Live). From
the project directory, compile the handbook three times to resolve its contents,
tables and cross-references:

```powershell
New-Item -ItemType Directory -Force build/handbook | Out-Null
foreach ($pass in 1..3) {
    pdflatex -interaction=nonstopmode -halt-on-error -output-directory=build/handbook docs/handbook/willowfield_codebase_handbook.tex
    if ($LASTEXITCODE -ne 0) { throw "Handbook build failed on pass $pass" }
}
Copy-Item -LiteralPath build/handbook/willowfield_codebase_handbook.pdf -Destination docs/handbook/willowfield_codebase_handbook.pdf
```

The source uses the farmer images under `screenshots/`, so keep the repository
root as the working directory. Build intermediates stay in ignored
`build/handbook/`. Review the compiler log for unresolved references and layout
warnings, then inspect the resulting PDF before committing it with the source.
LaTeX is not included in `format.ps1`.

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
`src/`, `docs/`, `scripts/`, `cmake/`, and `assets/text.txt`; other assets,
screenshots, dependencies, build outputs, and IDE settings are excluded.

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
