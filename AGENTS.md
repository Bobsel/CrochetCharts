# PROJECT KNOWLEDGE BASE

**Generated:** 2026-04-18
**Commit:** 198b9df
**Branch:** upgrade

## OVERVIEW

Desktop crochet chart editor. Qt 4.8 + C++11 (gnu++11), CMake build, QGraphicsScene-based canvas with SVG stitch rendering. 22,867 LOC across 99 C++ files. Legacy codebase (2010-2014 origin, Brian C. Milco / Stitch Works Software), GPLv3 / CC BY-SA 4.0 artwork.

## STRUCTURE

```
CrochetCharts/
├── src/            # 99 .cpp/.h + 14 .ui — flat (no subdirs). See src/AGENTS.md
├── tests/          # QtTest — 6 test classes, ~5% coverage. See tests/AGENTS.md
├── stitches/       # 129 SVG stitch icons + stitches.qrc. See stitches/AGENTS.md
├── cmake/modules/  # FindHunSpell, GetGitRevisionDescription, DocbookGen, NSIS template, version.cpp.in
├── images/         # Toolbar icons + macOS .iconset bundles (Crochet Charts.iconset, PatternDocument.iconset)
├── docs/
│   ├── architecture/  # arc42 software architecture doc (multi-file, mermaid). See docs/architecture/README.md
│   └── index.docbook.in  # End-user DocBook manual (HTML/PDF via fop + xsltproc)
├── resources/      # CPack configs, .desktop.in, MIME xml, macOS Entitlements.plist, deb/{postinst,prerm}
├── bin/            # Legacy bash scripts (build, tests, profile, setup) + gprof2dot.py + asan_symbolize.py
├── utils/hooks/    # pre-commit git hooks (bash)
├── .devcontainer/  # Ubuntu 18.04 Dockerfile + VS Code config (X11 forwarding via --net=host)
├── crochet.xml     # Default stitch set definition (~1000 lines, 100+ stitch entries)
├── crochet.qrc     # Main Qt resource collection
├── crochet.rc      # Windows exe icon resource
└── Taskfile.yml    # Task runner (preferred over bin/ scripts)
```

## WHERE TO LOOK

| Task | Location | Notes |
|------|----------|-------|
| Canvas/drawing/events | `src/scene.cpp` | **3626 lines, 142 methods — god class** |
| UI shell, menus, actions | `src/mainwindow.cpp` | 2043 lines, 116 methods, 32 includes |
| Chart cell (stitch instance) | `src/cell.{cpp,h}` | QGraphicsSvgItem; grants friend to SaveFile/File_v1/File_v2 |
| File I/O (versioned) | `src/file.h` + `file_v1.cpp` + `file_v2.cpp` + `filefactory.cpp` | FileFactory dispatches by version. **Uses `friend class` to reach into MainWindow, Scene, Cell, CrochetTab** |
| Undo stack | `src/crochetchartcommands.cpp` (+`.h` 449 lines, 18+ QUndoCommand subclasses) | Indicator undo separate: `indicatorundo.{cpp,h}` |
| Stitch data model | `src/stitch.{cpp,h}` (QObject) + `stitchset.{cpp,h}` (QAbstractItemModel) |  |
| Stitch library (global) | `src/stitchlibrary.{cpp,h}` | Singleton — `StitchLibrary::inst()` |
| Properties panel | `src/propertiesdock.cpp` | 737 lines |
| Export (PDF/SVG/PNG) | `src/exportui.cpp` | 697 lines |
| Layers | `src/chartLayer.{cpp,h}` | Plain C++ data class (no Qt base) |
| Error handler (msg hook) | `src/errorhandler.h` | Installed via `qInstallMsgHandler` in main |
| Settings singleton | `src/settings.{cpp,h}` | QSettings wrapper — `Settings::inst()` |
| App info singleton | `src/appinfo.{cpp,h}` | `AppInfo::inst()` |
| Stitch SVG asset | `stitches/*.svg` + entry in `crochet.xml` | Must add both — see stitches/AGENTS.md |
| Qt Designer forms | `src/*.ui` (14 files) | Processed via `qt4_wrap_ui` |
| Translation files | (none present) | `QT4_CREATE_TRANSLATION` commented out in root CMakeLists |
| Version string | Generated `build/version.cpp` from `cmake/modules/version.cpp.in` | `git describe --tags --dirty=w` drives `VERSION_MAJOR.MINOR.PATCH` |

## CODE MAP

| Symbol | Type | Location | Role |
|--------|------|----------|------|
| `main` | function | src/main.cpp:35 | Installs errorHandler → Application → MainWindow → exec |
| `Application` | QApplication | src/application.h:29 | File-open event queueing (macOS); inits StitchLibrary |
| `MainWindow` | QMainWindow | src/mainwindow.h:51 | Tab host, menu/toolbar owner |
| `Scene` | QGraphicsScene | src/scene.h:107 | Canvas, selection, undo dispatch, mouse-mode state machine |
| `CrochetTab` | QWidget | src/crochettab.h:42 | Owns Scene + ChartView per chart |
| `ChartView` | QGraphicsView | src/chartview.h:29 | Viewport (zoom/pan) |
| `Cell` | QGraphicsSvgItem | src/cell.h:28 | One stitch on canvas (SVG + color + rotation) |
| `Indicator` | QGraphicsTextItem | src/indicator.h:35 | Row/col numbering on canvas |
| `ChartImage` | QGraphicsObject | src/ChartImage.h:10 | Embedded bitmap on canvas |
| `Guideline` | QGraphicsEllipseItem | src/guideline.h:26 | Interactive grid guide |
| `ItemGroup` | QGraphicsItemGroup | src/itemgroup.h:26 | Group with layer tracking |
| `ColorLegend` / `StitchLegend` | QGraphicsWidget | src/legends.h:37 / :61 | Canvas legends |
| `StitchLibrary` | QObject (singleton) | src/stitchlibrary.h:46 | Built-in + user stitch sets |
| `StitchSet` | QAbstractItemModel | src/stitchset.h:37 | XML (de)serialization, MVC model |
| `FileFactory` | plain | src/filefactory.h | Dispatches open/save to versioned File_vN |

## ARCHITECTURE DOC

Architecture is documented in `docs/architecture/` as a 12-section arc42 doc (see `docs/architecture/README.md`). It is a **living document** and must stay in sync with the code — **update the doc in the same commit as the code change**.

Sync rules, trigger table, and workflow live in the `arc42-sync` skill at `.agents/skills/arc42-sync/SKILL.md`. Load that skill before editing architecture-relevant code.

## CONVENTIONS

- **Flat `src/`**: all 99 .cpp/.h + 14 .ui in one directory. No module grouping. Expected — do not refactor without coordinating.
- **File naming**: mostly camelCase (`chartLayer.cpp`, `crochettab.cpp`), mixed with PascalCase (`ChartImage.cpp`, `ChartItemTools.cpp`). Do not normalize.
- **Member prefix `m`**: `mHighlight`, `mType`, `mW`. Applies to all new members.
- **Versioned file format**: `file_vN.{cpp,h}` + `FileFactory`. When adding a new format, add `file_v3` alongside, do not edit v1/v2.
- **Singletons via `inst()` static**: `Settings::inst()`, `AppInfo::inst()`, `StitchLibrary::inst()`. Not thread-safe.
- **Friend class for I/O**: `Cell`, `Scene`, `MainWindow`, `CrochetTab`, `Indicator` grant `friend class File_v1/File_v2/FileFactory/SaveFile`. Tight coupling is intentional.
- **Custom `QGraphicsItem::Type = UserType + N`** scattered across Cell, Indicator, ChartImage, Guideline, ItemGroup. No central enum — check existing values before adding a new type to avoid collision.
- **QUndoCommand subclasses** are created with `new` and handed to `undoStack->push()` (stack takes ownership — this is Qt's pattern, not a leak).
- **Qt4 idioms only**: `SIGNAL(...)`/`SLOT(...)` string macros, `qInstallMsgHandler` (not `qInstallMessageHandler`), `qt4_add_resources`, `qt4_wrap_ui`. Do not mix Qt5 API.
- **Copy prevention**: `Q_DISABLE_COPY` is NOT used anywhere — copyability of model classes is accidental, not enforced.
- **Scene border**: `SCENE_CLAMP_BORDER_SIZE 50` define in `scene.h`.

## ANTI-PATTERNS (THIS PROJECT)

- **`scene.cpp` (3626 lines, 142 methods) is a god object** — do not add more logic here. New canvas behavior: consider extracting a helper class.
- **`mainwindow.cpp` (2043 lines, 116 methods, 32 includes)** is a secondary hub. Adding a new dock/dialog: keep the wiring minimal here.
- **Hardcoded Windows paths** in `src/CMakeLists.txt`: `C://qt//4.8.5//bin`, `C://MinGW-4.4//bin`. Do not use as reference — patch locally if needed.
- **`QString *data = new QString(); … delete data;`** pattern in `file_v1.cpp:115`, `file_v2.cpp:108`, `stitchset.cpp:273,314`. Use stack-allocated `QString` instead.
- **`//TODO transform_refactor`** markers in `scene.cpp` signal an incomplete refactor — inherit the intent when touching nearby code.
- **Known FIXMEs worth reading before touching**:
  - `stitchlibraryui.cpp:590` — "crashes when removing sets with sts in the master list"
  - `scene.cpp:540` — Ctrl-selection fix with unknown side effects
  - `mainwindow.cpp:955` — fileSave cancel still closes window
  - `updater.cpp:168` — crash on cancel
- **No Qt5/Qt6 port**: `find_package(Qt4 REQUIRED)` is mandatory. Installing Qt5-only distros breaks build.
- **Test coverage ~5%**: 6 test classes for ~100 app classes. Do not assume tests will catch regressions.

## COMMANDS

```bash
# Preferred (go-task)
task build          # Debug (cmake + make -j)
task build:release  # Release
task test           # Builds with -DUNIT_TESTING=ON, runs build/tests/tests
task run            # Build and launch
task profile        # Profile build + gprof + dot → profile.png
task clean          # Remove build/, build_release/, build_profile/
task setup          # Install git hooks (cp utils/hooks/* .git/hooks/)

# Legacy (still functional)
./bin/build [-t] [debug|release|installer]  # -t = include tests
./bin/tests
./bin/profile

# Manual
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUNIT_TESTING=ON
make -j$(nproc) && ./tests/tests
```

## NOTES

- **Out-of-source builds enforced** by `MacroOutOfSourceBuild.cmake` — running cmake in the root fails on purpose.
- **`CMAKE_AUTOMOC = TRUE`** — do not run moc manually; AUTOMOC scans all `Q_OBJECT` headers compiled into the target.
- **Optional deps**: Hunspell (spell check, `HUNSPELL_FOUND`), Doxygen (`-DDOXYGEN=ON`), DocBook toolchain (`-DDOCS=ON`).
- **Dev Container** expects host to run `xhost +local:` before launching GUI; uses `--net=host` and mounts `/tmp/.X11-unix`.
- **Git describe** drives version: commits must be tagged for `VERSION_MAJOR.MINOR.PATCH` to parse correctly; dirty working tree appends `w`.
- **Binary file format**: `QDataStream` (v1) / XML (v2) with embedded XML stitch definitions. Binary is NOT stable across Qt version upgrades.
- **No `.clang-format` / `.clang-tidy` / `.editorconfig`** — style is inherited from surrounding code only.
