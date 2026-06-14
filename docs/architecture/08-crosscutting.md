# 8. Crosscutting Concepts

Patterns applied in more than one place. Understand these once; they recur.

## 8.1 Undo via Command Pattern

Every mutating user action funnels through a `QUndoCommand` subclass. `src/crochetchartcommands.h` defines 21 of them (e.g. `SetCellStitch`, `SetItemRotation`, `SetItemCoordinates`, `AddItem`, `RemoveItem`, `GroupItems`, `UngroupItems`, plus a `SetLayer*` family). Indicator mutations have a parallel set of 3 in `src/indicatorundo.{cpp,h}` (`AddIndicator`, `RemoveIndicator`, `ChangeTextIndicator`).

Rules:
- **No direct Scene mutation.** If the user can undo it, it goes through a command.
- **Ownership:** commands are `new`-ed and pushed onto the stack; the stack deletes them.
- **First-push executes.** `QUndoStack::push()` calls `redo()` once immediately.
- **Composite edits** use a single command that stores a list of (before, after) pairs, not multiple pushes; this keeps "Undo" atomic from the user's point of view.

## 8.2 Singletons

Three services are singletons accessed via a static `inst()` method:

| Singleton | Purpose | Lifetime |
|---|---|---|
| `Settings::inst()` | Thin `QSettings` wrapper | Matches `QApplication` |
| `AppInfo::inst()` | Version, build info (from `version.cpp`) | Process |
| `StitchLibrary::inst()` | Global catalog of stitch sets | Process |

Constraints:
- Accessed only from the GUI thread. Not thread-safe.
- Initialised lazily on first `inst()` call (see `Application` bootstrap).
- Shutdown order is implicit. Do not rely on singletons inside destructors called after `QApplication` dies.

## 8.3 Friend-based Serialization

The versioned file classes (`File_v1`, `File_v2`, `SaveFile`, `FileFactory`) declare `friend class …` on the data classes they serialise: `Cell`, `Scene`, `CrochetTab`, `Indicator`, `MainWindow`. This lets serializers read and write private fields directly without dedicated accessors.

Pros: no ceremony for new fields — add a private member and touch `File_vN`.
Cons: encapsulation is effectively zero for these classes; every serializer is a transitive dependency on every data class it touches.

**When adding a new persisted field:** update both `File_v2` (write + read) *and* the relevant test in `tests/filetest.cpp`. Do not edit `File_v1` — it is frozen for backwards compatibility.

See the subsection below for the on-disk shape.

### 8.3.1 Project file format

| Version | Extension | Shape | Notes |
|---|---|---|---|
| v1 | `.crochetcharts` | Raw `QDataStream` with fixed ordering of fields | Not portable across Qt builds with different `QDataStream::Version`. Freeze — only read path is maintained. |
| v2 | `.crochetcharts` | QDataStream framing → embedded XML document → binary blobs for icon bytes | Current default. Self-contained: includes stitch-icon blobs so the file renders without the originating user's `.set` library. |

`FileFactory` dispatches by reading the magic/version header. See [06-runtime.md § 6.2](06-runtime.md#62-opening-a-project-file) for the sequence.

### 8.3.2 Stitch set format

`.set` files (see `src/stitchset.cpp`) are XML + a sidecar binary blob of icon bytes. Stored under the user data directory. The StitchLibrary enumerates this directory on startup.

## 8.4 Qt Resource System

Two `.qrc` files:
- `crochet.qrc` — UI chrome: toolbar icons, splash, watermark, `license.txt`, `crochet.xml`. Prefix `/`.
- `stitches/stitches.qrc` — 129 stitch SVGs. Prefix `/stitches`.

Consequences:
- Built-in stitches are **compiled in** — modifying an SVG requires a rebuild.
- User-supplied stitch sets live on disk under the user directory and are loaded by name at runtime. Adding a stitch without rebuilding means supplying a user `.set`.
- All resource paths in code look like `:/stitches/dc.svg` or `:/crochet.xml`.

## 8.5 Mode-based Interaction

`Scene::mMode` (type `Scene::EditMode`) selects behaviour for mouse and keyboard events. Modes are effectively sub-tools: `StitchEdit`, `ColorEdit`, `RowEdit`, `RotationEdit`, `ScaleEdit`, `IndicatorEdit`. See `src/scene.h:118-125`. A separate `Scene::SelectMode` (`BoxSelect`, `LassoSelect`, `LineSelect`) further refines selection behaviour and is held in `mSelectMode`.

**Pros:** one class handles all interaction, consistent selection semantics.
**Cons:** `Scene::mousePressEvent` is a multi-branch `switch` that grows with each mode. This is the main reason `scene.cpp` is a god class.

Guidelines for adding a mode:
1. Add a new value to the mode enum.
2. Add a toolbar action in `MainWindow` that sets the mode.
3. Add branches in the three mouse handlers.
4. Add one or more `QUndoCommand` subclasses for any persisted effects.
5. Extend the existing test for mode transitions in `tests/`.

## 8.6 Error handling

Custom Qt message handler installed via `qInstallMsgHandler(errorHandler)` in `main()`. `errorhandler.h` shows `qFatal` and `qCritical` messages via `QMessageBox`; `qWarning` falls through to stderr.

**Exceptions are not used.** Error paths return `bool`/error codes and usually `qCritical` on failure. A new subsystem should follow the same style.

## 8.7 SVG rendering and colour substitution

`Cell` is a `QGraphicsSvgItem` that loads a stitch's SVG once per unique stitch. Colouring is performed by in-memory string substitution on the SVG source before handing it to `QSvgRenderer`. See `cell.cpp`.

Implication: SVGs used as stitch icons must conform to the placeholder colour convention expected by the substitution — check existing SVGs in `stitches/` before adding new ones. See `stitches/AGENTS.md`.

## 8.8 Threading model

There isn't one. All logic runs on the GUI thread. Network operations in `updater.cpp` are evented (Qt signals) but not threaded. Do not introduce threads without serious reason; singletons and `Scene` are not thread-safe.

## 8.9 Logging

There is no framework. The code uses raw `qDebug()` / `qWarning()` calls, which are captured by the installed message handler. There is no log-level configuration and no log file — everything goes to stderr or (for critical) to a dialog.

## 8.10 Testing

`QtTest`-based unit tests under `tests/` cover a thin slice: 8 `QObject` test classes — `TestCell`, `TestStitch`, `TestStitchSet`, `TestStitchLibrary`, `TestSettings`, `TestTextView`, `TestChartView`, `TestFileFactory`. The last two are port-regression fixtures (Qt4-frozen golden masters) intended to catch binary-format breakage during a future Qt5/6 port. Coverage is ≈ 5 %. Tests run headless under `xvfb-run` in the dev container.

- Tests are **not** a reliable regression net. Changes to `Scene`, docks, or dialogs rarely have a failing test before the bug is reported.
- Generated test artifacts (PNG diffs, `.set` roundtrips) are git-ignored; see `tests/.gitignore`.

See `tests/AGENTS.md` for conventions on writing new tests.
