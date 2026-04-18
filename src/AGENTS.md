# src/ — Application Source

Flat directory, 99 .cpp/.h + 14 .ui + CMakeLists.txt. No subdirectories. See root AGENTS.md for build, conventions, global patterns.

## LAYOUT (by role, not by filesystem)

```
Entry:        main.cpp → application.{cpp,h} → mainwindow.{cpp,h}
Canvas core:  scene.{cpp,h}  crochettab.{cpp,h}  chartview.{cpp,h}
Items:        cell.{cpp,h}  indicator.{cpp,h}  ChartImage.{cpp,h}  guideline.{cpp,h}  itemgroup.{cpp,h}  legends.{cpp,h}
Items-tools:  ChartItemTools.{cpp,h}  chartLayer.{cpp,h}  selectionband.{cpp,h}
Stitch data:  stitch.{cpp,h}  stitchset.{cpp,h}  stitchlibrary.{cpp,h}  stitchlibrarydelegate.{cpp,h}  stitchpalettedelegate.{cpp,h}
Stitch UI:    stitchlibraryui.{cpp,h,ui}  stitchiconui.{cpp,h,ui}  stitchreplacerui.{cpp,h,ui}
Undo:         crochetchartcommands.{cpp,h}  indicatorundo.{cpp,h}  undogroup.{cpp,h}
File I/O:     file.{cpp,h}  file_v1.{cpp,h}  file_v2.{cpp,h}  filefactory.{cpp,h}
Docks:        aligndock.{cpp,h,ui}  rowsdock.{cpp,h,ui}  mirrordock.{cpp,h,ui}  propertiesdock.{cpp,h,ui}
Dialogs:      exportui.{cpp,h,.ui=export.ui}  resizeui.{cpp,h,.ui=resize.ui}  colorreplacer.{cpp,h,ui}  roweditdialog.{cpp,h,ui}
Settings:     settings.{cpp,h,ui}  settingsui.{cpp,h}  appinfo.{cpp,h}
Misc:         splashscreen.{cpp,h}  textview.{cpp,h}  updater.{cpp,h}  debug.{cpp,h}
              colorlabel.{cpp,h}  colorlistwidget.{cpp,h}  colorreplacer.{cpp,h,ui}
Interfaces:   tabinterface.h  errorhandler.h  updatefunctions.h  version.h  propertiesdata.{cpp,h}
Build:        CMakeLists.txt   (file(GLOB "*.cpp") — new .cpp files picked up automatically)
```

## COMPLEXITY HOTSPOTS

| File | Lines | Methods | Includes | Treat as |
|------|-------|---------|----------|----------|
| scene.cpp | 3626 | 142 | 24 | **God class** — do not grow |
| mainwindow.cpp | 2043 | 116 | 32 | **App hub** — keep wiring thin |
| file_v2.cpp | 921 | — | — | Serializer (friend-class intrusion into Scene/Cell) |
| propertiesdock.cpp | 737 | 38 | — | Dialog-like dock |
| exportui.cpp | 697 | — | — | Export dialog |
| stitchset.cpp | 647 | 35 | — | XML (de)serializer + item model |
| stitchlibraryui.cpp | 610 | — | — | Library management dialog |
| crochetchartcommands.{cpp,h} | 597 + 449 | 76 | — | 18+ QUndoCommand subclasses |
| file_v1.cpp | 558 | — | — | Legacy QDataStream format |

## RULES

- **CMakeLists uses `file(GLOB "*.cpp")`** — adding a `.cpp` needs a CMake re-run but not an edit. `.ui` files auto-discovered by `qt4_wrap_ui`.
- **Each `Q_OBJECT` class needs a matching `.h`** (AUTOMOC scans headers only).
- **Custom QGraphicsItem `Type`** — when adding a new `QGraphicsItem` subclass, pick a `UserType + N` not already used by Cell, Indicator, ChartImage, Guideline, ItemGroup.
- **When modifying `Cell`, `Scene`, `MainWindow`, `CrochetTab`, `Indicator`**: file_v1.cpp and file_v2.cpp reach into private members via `friend class`. Renaming a private field silently breaks save/load.
- **Adding a dock**: pattern is `{name}dock.{cpp,h,ui}` (Q_OBJECT class deriving `QDockWidget`), wired in `MainWindow::setupDocks`.
- **Singletons** (`Settings`, `AppInfo`, `StitchLibrary`) must not be touched from threads other than GUI thread.

## ANTI-PATTERNS (LOCAL)

- **`QString *data = new QString();`** (`file_v1.cpp:115`, `file_v2.cpp:108`, `stitchset.cpp:273,314`). Replace with stack QString in new code.
- **`QDrag`/`QMimeData` leak**: `colorlistwidget.cpp:64-65` creates `QMimeData` with no parent. Don't copy this pattern.
- **`//TODO transform_refactor`** in `scene.cpp` marks an abandoned in-progress refactor — preserve semantics, don't invent new.
- **`WARN("TODO: …")`** (`propertiesdock.cpp:374`) is a runtime warning, not a comment.

## TESTING FROM HERE

Unit tests live in `../tests/` and link the *entire* src/ (via explicit `crochet_app_cpp` list in `tests/CMakeLists.txt`, **not** a glob). Adding a new .cpp to src/ does NOT automatically add it to the tests target — edit `tests/CMakeLists.txt` explicitly.
