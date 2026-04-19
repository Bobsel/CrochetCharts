# Crochet Charts

Desktop crochet chart editor built with Qt4 and C++11. Uses QGraphicsScene for canvas rendering and SVG for stitch icons.

This software is licensed under GPLv3. All original artwork is licensed under Creative Commons Attribution-ShareAlike ( cc by-sa 4.0 ).

## Quick Start (Recommended)

The easiest way to get a working development environment is via the included **Dev Container** (requires Docker and VS Code):

1. Open the project in VS Code
2. `Ctrl+Shift+P` → **Dev Containers: Reopen in Container**
3. Once inside the container:

```bash
task build    # Debug build
task run      # Build and launch the app
```

> **Note:** To display the GUI on your host, run `xhost +local:` on your host terminal before launching.

### Available Tasks

| Command | Description |
|---------|-------------|
| `task build` | Debug build (CMake + make) |
| `task build:release` | Release build |
| `task run` | Build and launch the app |
| `task test` | Build with unit tests and run them |
| `task clean` | Remove all build artifacts |
| `task setup` | Install git hooks |
| `task profile` | Profile build, run, and generate call graph |
| `task package:linux` | Release build + `.deb` / `.rpm` installers (inside the Linux devcontainer) |
| `task images:build:win` | Build the MXE Windows cross-compile Docker image (host, ~30-60 min first time) |
| `task package:win` | Cross-compile a Windows NSIS installer (host, uses the image above) |
| `task artifacts:clean` | Remove `./artifacts` and package build dirs |

## Building installers

Artifacts end up in `./artifacts/`. The pipeline uses CPack generators configured in `resources/installers.cmake`.

### Linux (.deb + .rpm)

Run **inside the Linux devcontainer**:

```bash
task package:linux
# -> artifacts/CrochetCharts-<ver>-Linux.deb
# -> artifacts/CrochetCharts-<ver>-Linux.rpm
```

Requires `rpm` + `inkscape` (already in `.devcontainer/Dockerfile`). On a plain host, install Qt 4.8 + DocBook toolchain first.

### Windows (NSIS .exe, cross-compiled)

Run on the **host** (not inside the devcontainer — the Windows cross-build uses its own sibling image):

```bash
task images:build:win     # one-time, 30-60 min: builds MXE + Qt4 + NSIS
task package:win
# -> artifacts/Crochet_Charts-<ver>-win64.exe
```

The image is based on `debian:bookworm-slim` with [MXE](https://mxe.cc/) targeting `x86_64-w64-mingw32.static`. Qt is linked statically so the installer is self-contained — no DLLs to ship alongside. `MXE_REF` is pinned in `.devcontainer/win/Dockerfile`; bump via `--build-arg MXE_REF=<sha>` if upstream breaks.

### macOS

Only from a Mac host — see `README.md` > "Manual Setup" > "Mac" below.

## Manual Setup

If you prefer not to use the Dev Container, you'll need the following dependencies:

### All Platforms

+ Qt 4.8 (not Qt5/Qt6)
+ CMake (≥ 2.8.6)
+ docbook, docbook-xsl-ns (for documentation)
+ Apache fop, xsltproc (for documentation)

### Windows

+ MinGW
+ NSIS
+ signtool

### Mac

+ Xcode
+ (Paid) Mac Developer account (to sign the software)
+ Custom build of cmake - https://github.com/iPenguin/cmake

### Linux

+ build-essential
+ gcc and/or clang
+ doxygen (optional)
+ libhunspell-dev (optional, for spell checking)

### Building Manually

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./src/CrochetCharts
```

## Legacy Scripts

The `bin/` directory contains the original build scripts:

- `build` — compile the software based on the selected options (`-h` for a list of options)
- `tests` — run the unit tests and report the results
- `profile` — compile and run the software with profiling turned on
- `setup` — install git commit hooks

