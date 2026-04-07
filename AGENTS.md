# PROJECT KNOWLEDGE BASE

**Generated:** 2026-04-07
**Commit:** e9e99e0
**Branch:** master

## OVERVIEW

Desktop crochet chart editor. Qt4 + C++11, CMake build system, QGraphicsScene-based canvas with SVG stitch rendering.

## STRUCTURE

```
CrochetCharts/
├── src/           # All application source (113 .cpp/.h files, flat — no subdirs)
├── stitches/      # 130 SVG stitch icons + stitches.qrc
├── tests/         # Qt4 unit tests (cell, stitch, stitchset, stitchlibrary, settings, textview)
├── bin/           # Build/test/profile shell scripts
├── cmake/modules/ # Custom CMake modules (FindHunSpell, version.cpp.in, etc.)
├── images/        # App icons (SVG sources + .iconset dirs)
├── docs/          # Docbook documentation + images
├── resources/     # Installer configs (CPack, .deb)
├── utils/hooks/   # Git hooks
├── crochet.xml    # Default stitch set definition (XML)
└── crochet.qrc    # Main Qt resource file
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Canvas/drawing logic | `src/scene.cpp` (3626 lines) | Core QGraphicsScene — largest file by far |
| Main UI/menus/actions | `src/mainwindow.cpp` | 2043 lines, orchestrates everything |
| Chart cells (stitch items) | `src/cell.cpp`, `src/cell.h` | QGraphicsSvgItem subclass |
| File I/O | `src/file.h`, `src/file_v1.cpp`, `src/file_v2.cpp` | Versioned format via FileFactory |
| Undo system | `src/crochetchartcommands.cpp` | QUndoStack commands |
| Stitch library/sets | `src/stitchlibrary.cpp`, `src/stitchset.cpp` | Loading/managing SVG stitch definitions |
| Properties panel | `src/propertiesdock.cpp` | 736 lines, item property editing |
| Export | `src/exportui.cpp` | 697 lines |
| Layers | `src/chartLayer.cpp` | Recently added feature |
| UI forms | `src/*.ui` (14 files) | Qt Designer XML |
| Add new stitches | `stitches/` + `crochet.xml` | SVG + XML definition |

## CONVENTIONS

- **Flat src/**: All source in one directory, no module subdirs
- **Naming**: camelCase files (`chartLayer.cpp`), some PascalCase (`ChartImage.cpp`) — inconsistent
- **File versioning**: `file_v1.cpp`, `file_v2.cpp` pattern for format evolution
- **Qt4 idioms**: `Q_OBJECT` macros, signals/slots, `QGraphicsItem::Type` enum pattern
- **Friend classes**: `Cell` grants friend access to `SaveFile`, `File_v1`, `File_v2`
- **Prefix `m`**: Member variables use `m` prefix (`mHighlight`, `mType`)
- **Scene border**: `SCENE_CLAMP_BORDER_SIZE 50` define in scene.h

## ANTI-PATTERNS (THIS PROJECT)

- Multiple TODOs/FIXMEs scattered in source — treat as known tech debt, not bugs
- `scene.cpp` at 3626 lines is a god object — avoid adding more logic there
- Some hardcoded Windows paths in `src/CMakeLists.txt` (`C://qt//4.8.5//bin`)
- `//TODO transform_refactor` comments in scene.cpp indicate incomplete refactor

## COMMANDS

```bash
# Build (from project root)
./bin/build debug       # Debug build with ASan
./bin/build release     # Release build
./bin/build -t debug    # Build with unit tests

# Tests (after building with -t)
./bin/tests

# Manual build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

## NOTES

- Requires Qt 4.8 (not Qt5/Qt6) — `find_package(Qt4 REQUIRED)` in CMakeLists
- Optional Hunspell dependency (spell checking)
- SVG rendering via QtSvg module
- File format is binary QDataStream-based with XML stitch definitions
- Cross-platform: Linux, macOS, Windows (MinGW)
- GPLv3 licensed, artwork CC BY-SA 4.0
- Author: Brian C. Milco / Stitch Works Software (2010-2014)
