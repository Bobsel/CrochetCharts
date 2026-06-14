# 4. Solution Strategy

The load-bearing design choices, stated plainly. Each is expanded in later sections.

## 4.1 Key decisions

| # | Decision | Consequence |
|---|----------|-------------|
| S1 | **Canvas as `QGraphicsScene`** with each stitch a `QGraphicsSvgItem` subclass (`Cell`). | Free hit-testing, z-ordering, transforms, and animation. Locks us to Qt's scene-graph memory model. |
| S2 | **Document = `CrochetTab`** containing one `Scene` + `ChartView`. `MainWindow` is a tab host. | Multi-chart documents fall out naturally. Tab teardown must cascade-delete scene contents. |
| S3 | **Stitch catalog decoupled from chart data**. `StitchLibrary` is a singleton owning `StitchSet`s (each backed by XML + SVG assets). `Cell` refers to a `Stitch` by pointer. | Catalog can be extended at runtime. Dangling pointers if a stitch is removed while cells reference it — mitigated by library immutability at runtime. |
| S4 | **Versioned file format with factory dispatch**. `FileFactory` reads the header and constructs `File_v1` or `File_v2`. Future `v3` plugs in without touching v1/v2. | Saved files from 2010 still load. Serializer is `friend` of every data class it touches — encapsulation cost. |
| S5 | **Undo via Qt's `QUndoStack`** with 21 typed `QUndoCommand` subclasses in `src/crochetchartcommands.h` plus 3 indicator-specific commands in `src/indicatorundo.h`. | Uniform undo/redo across every mutating operation. Every new mutator requires a matching command class. |
| S6 | **Resources embedded via Qt resource system** (`crochet.qrc`, `stitches/stitches.qrc`). | Single binary, no asset-path wrangling at runtime. Adding a stitch requires a rebuild **unless** it is supplied as a user `.set`. |
| S7 | **Cross-platform packaging via CPack** generators (NSIS / Bundle+DMG / DEB+RPM) driven from `resources/installers.cmake`. | One CMake invocation per platform. Platform-specific assets (plist, entitlements, `.desktop`, MIME XML, `.rc`) are version-controlled. |
| S8 | **GUI-thread-only execution model**. No worker threads; long-running work (export, save) runs inline with busy cursor. | Code is simple; large exports block the UI. Acceptable for charts ≤ several thousand stitches. |
| S9 | **Settings through `QSettings` via a singleton wrapper** (`Settings::inst()`). | Consistent API, OS-native store. Singleton lifecycle coupled to `QApplication`. |

## 4.2 What was deliberately NOT done

- **No MVC separation** between chart data and Scene. Scene *is* the model. Data and presentation are the same objects (`Cell` is both a `QGraphicsSvgItem` and the domain entity).
- **No threading.** File I/O, rendering, and export are synchronous.
- **No dependency injection.** Services are reached via singletons or direct ownership.
- **No plugin system.** All stitches ship in the binary or in the user's `.set` directory; extensibility stops there.
- **No Qt5/6 port path.** See ADR-01.

These simplifications are the reason a solo maintainer can still navigate the code.

## 4.3 Quality-goal → strategy mapping

| Quality goal (from § 1.3) | Strategy element |
|---|---|
| Correctness of saved files | S4 (versioned + frozen past versions), plus test class `FileTest` |
| Interactive responsiveness | S1 (Qt scene graph hit-testing), S8 (no IPC overhead) |
| Cross-platform parity | S6 (embedded assets), S7 (CPack), Qt4 abstraction |
| Installability by non-tech users | S7 (platform-native installers with icons, MIME, desktop files) |
| Extensibility of stitch catalog | S3 (pointer-to-Stitch), S6 (user-directory `.set` loading) |
| Maintainability | Partially served by S2, S4, S5; eroded by S1+S8 coupling (scene god class). Tracked as debt. |
