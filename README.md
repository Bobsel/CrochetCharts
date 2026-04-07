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

