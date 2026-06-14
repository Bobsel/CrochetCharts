# 12. Glossary

Terms that recur in this document and the codebase. Scoped to this project; general Qt/C++ terms only when overloaded or easy to confuse.

## Domain

| Term | Meaning |
|---|---|
| **Chart** | A single crochet diagram — one canvas of cells. A project file holds one or more charts. |
| **Stitch** | A crochet operation: chain (ch), single crochet (sc), double crochet (dc), etc. In the code, a `Stitch` is an entry in a `StitchSet`. |
| **Stitch Set** | A named collection of stitches shipped as XML + SVG icons. Built-in (`:/stitches`) or user-supplied (`.set`). |
| **Stitch Library** | Process-global catalog of all loaded stitch sets. `StitchLibrary::inst()`. |
| **Cell** | One placed stitch on a chart canvas. A `QGraphicsSvgItem` subclass holding a `Stitch*`, colour, and transform. |
| **Indicator** | Text marker on the canvas (row/column numbers etc.). A `QGraphicsTextItem` subclass. |
| **Project file** | `.crochetcharts` file containing all charts of a document. Versioned on disk. |
| **Chart image** | An embedded raster image on the canvas, rendered by `ChartImage`. |
| **Legend** | On-canvas key mapping colours or stitches to labels. `ColorLegend`, `StitchLegend`. |
| **Guideline** | Interactive elliptical guide used while placing stitches in a round. `Guideline`. |
| **Row** | A logical grouping of cells for numbering / editing. Managed by `RowsDock`. |
| **Mode** | The interaction sub-tool active in the canvas (stitch, rotate, scale, insert, colour …). Stored in `Scene::mMode`. |

## Technical

| Term | Meaning |
|---|---|
| **`QGraphicsScene` / `QGraphicsView`** | Qt's 2D scene-graph pair. We subclass `QGraphicsScene` as `Scene` and `QGraphicsView` as `ChartView`. |
| **`QGraphicsItem::Type`** | Qt's mechanism for runtime type distinction of scene items. We use `UserType + N` values (see D-8). |
| **`QUndoStack` / `QUndoCommand`** | Qt's command pattern. Every mutator is a command class (§ 8.1). |
| **`qt4_add_resources` / `qt4_wrap_ui`** | CMake macros from Qt4's `UseQt4.cmake`, used to compile `.qrc` and `.ui` files. |
| **`Q_OBJECT` / AUTOMOC** | Qt's meta-object macro + our CMake `CMAKE_AUTOMOC = TRUE`. |
| **SIGNAL / SLOT** | Qt4 string-macro connect syntax. Project uses only Qt4 style. |
| **QtTest** | Qt's unit-test framework used under `tests/`. |
| **CPack** | CMake's packaging sub-tool. We use generators NSIS, Bundle+DragNDrop, DEB, RPM. |
| **NSIS** | Nullsoft Scriptable Install System — Windows installer generator. |
| **DocBook** | XML markup language used by `docs/index.docbook.in` for the end-user manual. |
| **`.desktop` file** | XDG desktop entry used on Linux to install the app into the applications menu. |
| **UTI / MIME** | macOS Uniform Type Identifier / Linux MIME — how the OS associates `.crochetcharts` with the app. See `resources/vnd.stitchworks.pattern.xml`. |
| **`QSettings`** | Qt's cross-platform settings store (registry / plist / INI). Wrapped by `Settings::inst()`. |
| **arc42** | The template this document follows. See https://arc42.org. |
| **ADR** | Architecture Decision Record — individual entries in [09-decisions.md](09-decisions.md). |

## Paths

| Path | What lives there |
|---|---|
| `src/` | Flat application source — 99 `.cpp/.h`, 14 `.ui`. See `src/AGENTS.md`. |
| `tests/` | QtTest unit tests. See `tests/AGENTS.md`. |
| `stitches/` | 129 SVG stitch icons + `stitches.qrc`. See `stitches/AGENTS.md`. |
| `cmake/modules/` | `FindHunSpell`, `GetGitRevisionDescription`, `DocbookGen`, NSIS template, `version.cpp.in`. |
| `images/` | Toolbar icons + macOS `.iconset` bundles (`Crochet Charts.iconset`, `PatternDocument.iconset`). |
| `docs/` | DocBook user guide source. `docs/architecture/` — this document. |
| `resources/` | CPack configs, `.desktop.in`, MIME XML, macOS Entitlements, DEB `postinst/prerm`. |
| `bin/` | Legacy bash scripts (`build`, `tests`, `profile`, `setup`) + `gprof2dot.py` + `asan_symbolize.py`. Superseded by `Taskfile.yml`. |
| `utils/hooks/` | Git pre-commit hooks installed by `task setup`. |
| `.devcontainer/` | Dockerfile + devcontainer.json for the dev environment. |
| `crochet.xml` | Default stitch set definition (100+ stitches). |
| `crochet.qrc` | Main Qt resource manifest. |
| `crochet.rc` | Windows `.exe` icon resource. |
| `Taskfile.yml` | Task runner — preferred entry point for dev loop. |
