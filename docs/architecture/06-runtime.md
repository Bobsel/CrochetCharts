# 6. Runtime View

Key scenarios as sequence diagrams. Coverage is deliberately partial — scenarios chosen are the ones new contributors ask about.

## 6.1 Application startup

```mermaid
sequenceDiagram
    participant OS
    participant main as main()
    participant App as Application
    participant EH as errorHandler
    participant SL as StitchLibrary
    participant MW as MainWindow

    OS->>main: exec
    main->>EH: qInstallMsgHandler(errorHandler)
    main->>App: new Application(argc,argv)
    App->>SL: inst()  (lazy init)
    SL->>SL: load built-in crochet.xml + stitches from :/stitches
    SL->>SL: scan user set directory
    main->>MW: new MainWindow
    MW->>MW: buildMenus · buildDocks · restoreGeometry
    main->>App: exec()
    App-->>OS: event loop
```

Notable:
- The message handler is installed before `QApplication` construction so even Qt-internal warnings route through `errorhandler`.
- On macOS, `Application` queues `QFileOpenEvent`s that arrive before `MainWindow` exists and replays them once it does. See `src/application.cpp`.

## 6.2 Opening a project file

```mermaid
sequenceDiagram
    participant User
    participant MW as MainWindow
    participant FF as FileFactory
    participant FV as File_v2
    participant Tab as CrochetTab
    participant Scene
    participant SL as StitchLibrary

    User->>MW: File → Open
    MW->>MW: QFileDialog → path
    MW->>FF: open(path)
    FF->>FF: read header, detect version
    FF->>FV: new File_v2(path)
    FV->>Tab: new CrochetTab per chart
    Tab->>Scene: new Scene
    FV->>Scene: for each <cell> element
    FV->>SL: lookup Stitch by name
    SL-->>FV: Stitch*
    FV->>Scene: addItem(new Cell(stitch))
    FV-->>MW: success / error
    MW->>MW: addTab · focus last
```

- `File_v2` reaches into private members of `Scene`, `Cell`, `CrochetTab` via its friend grant. This is the intentional encapsulation break (§ 2.3).
- Unknown stitches fall back to `unknown.svg`; the cell keeps its name so a later stitch-set install resolves it.

## 6.3 Placing a stitch

The most frequent interactive path. Shown for "stitch mode".

```mermaid
sequenceDiagram
    participant User
    participant View as ChartView
    participant Scene
    participant Undo as QUndoStack
    participant Cmd as AddCell
    participant Cell

    User->>View: click on canvas
    View->>Scene: mousePressEvent
    Scene->>Scene: branch on mMode == StitchEdit
    Scene->>Cmd: new AddCell(scene, stitch, pos)
    Scene->>Undo: push(cmd)
    Undo->>Cmd: redo()  (first push executes)
    Cmd->>Cell: new Cell(stitch)
    Cmd->>Scene: addItem(cell)
    Cell-->>Scene: itemChange → select
    Scene-->>View: update()
```

Every mutator follows this shape:
1. Scene decides what to do based on `mMode`.
2. Scene constructs a typed `QUndoCommand` subclass.
3. The command's `redo()` performs the mutation; `undo()` reverses it.
4. Pushing onto the stack *automatically* calls `redo()` the first time.

Skipping the command path — mutating `Scene` directly — breaks undo/redo and is a bug.

## 6.4 Saving a project file

```mermaid
sequenceDiagram
    participant User
    participant MW as MainWindow
    participant SaveFile
    participant FV as File_v2
    participant Tab as CrochetTab
    participant Scene

    User->>MW: File → Save
    MW->>MW: path = dirty? existing : QFileDialog
    MW->>SaveFile: save(path, project)
    SaveFile->>FV: new File_v2(path, write)
    loop for each tab
        FV->>Tab: read chart metadata
        FV->>Scene: iterate items
        Scene-->>FV: Cell, Indicator, ChartImage, ItemGroup
        FV->>FV: emit <cell …/> with transform, colour, stitch ref
    end
    FV->>FV: flush XML + embedded binary blobs
    FV-->>SaveFile: ok
    SaveFile-->>MW: mark clean, update title
```

- Save is synchronous on the GUI thread. For realistic charts it completes in tens of milliseconds; for pathological ones it can stutter. See quality scenario QS-2 in [10-quality.md](10-quality.md).
- `v2` persists embedded icon binaries so a project opened on a machine without the stitch set still renders correctly.

## 6.5 Export to PDF

```mermaid
sequenceDiagram
    participant User
    participant MW as MainWindow
    participant UI as ExportUI
    participant Scene
    participant Printer as QPrinter

    User->>MW: File → Export
    MW->>UI: show modal (pick format, scale, pages)
    User->>UI: OK
    UI->>Printer: set output=path, format=PDF
    UI->>Scene: render(&painter, target, source)
    Scene-->>Printer: rasterised / vector ops
    Printer-->>UI: file written
```

`QPrinter` in Qt4 handles PDF natively; SVG export goes through `QSvgGenerator` similarly; PNG through `QImage`. See `src/exportui.cpp`.

## 6.6 Crash path — message handler

```mermaid
sequenceDiagram
    participant Qt as Qt runtime
    participant EH as errorHandler()
    participant User

    Qt->>EH: qWarning / qCritical / qFatal
    alt qFatal or critical
        EH->>User: QMessageBox with message + build info
        EH->>EH: abort / exit
    else qWarning
        EH->>EH: log to stderr only
    end
```

Installed early in `main()`; ensures even pre-`MainWindow` Qt errors produce a dialog rather than silent stderr chatter.
