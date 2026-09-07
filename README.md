# Willowfield

Interactive 3D Farm Environment with Dynamic Vegetation and Structural
Deformation. Built with C++20, OpenGL and FreeGLUT for CSC3081.

**Author:** Sivothayan .S

**Registration number:** S/21/513

![Farm overview](screenshots/overview.png)

## Features

- Explore the farm with a movable perspective camera.
- Adjust wind that bends grass, wheat, branches and leaves while keeping roots
  fixed.
- Cut meadow grass and restore it to its original height.
- Animated cows and windmill, farm structures, lighting, textures and day/night.

## Build and run

On Windows, install Visual Studio 2026 with **Desktop development with C++**,
the Windows SDK and CMake tools. The solution uses the **v145** toolset. The
first build downloads FreeGLUT 3.8.0 and verifies its SHA-256 checksum, so
internet access is required. Later builds reuse the cache under ignored
`build/`. Textures are included.

Open `csc3081_farm_project.slnx`, select **Release | x64**, and press
**Ctrl+F5**. Alternatively, run from the project directory:

```powershell
.\scripts\build.ps1 -Configuration Release
.\build\x64\Release\csc3081_farm_project.exe
```

CMake is also supported from a Developer PowerShell:

```powershell
cmake -S . -B build/cmake -A x64
cmake --build build/cmake --config Release --parallel
.\build\cmake\Release\csc3081_farm_project.exe
```

Windows x64 and Win32 Debug have been tested. Other platforms are unverified.
Keep `assets/` and the generated `FREEGLUT-LICENSE.txt` beside the executable
when copying a build. Missing textures use fallback colors.

## Controls

| Input               | Action                           |
| ------------------- | -------------------------------- |
| W / A / S / D       | Move                             |
| Arrow keys          | Look around                      |
| Right mouse drag    | Look around                      |
| Q / E               | Lower / raise camera             |
| G / V               | Meadow view / overview           |
| + or = / -          | Increase / decrease wind         |
| 0 / 1 / 2 / 3       | Wind off / low / medium / strong |
| C / R               | Toggle cutting / restore grass   |
| P                   | Pause animation and cutting      |
| N                   | Toggle day/night                 |
| B                   | Toggle the bend diagram          |
| L / T / F           | Lighting / textures / wireframe  |
| Esc or window close | Exit                             |

Press **G**, then **C**, and walk through the meadow to cut grass. Cutting works
near ground level; shortened grass remains until **R** is pressed.

## Development

Code is organized under `src/` by scene component. Vegetation displacement
scales with squared height, so the base stays fixed and the tip bends more.
Camera bounds prevent leaving the area or moving below ground; objects do not
block movement.

Run the build, behavior, rendering and window-close checks with:

```powershell
.\scripts\verify.ps1
```

Run `.\scripts\format.ps1` to format the project, or add `-Check` to check
formatting. See [scripts and formatting](docs/SCRIPTS.md) for prerequisites,
editor setup and generated files. Build outputs belong in the ignored `build/`
directory.

See [third-party resources](docs/THIRD_PARTY.md) for dependency licenses and
references.
