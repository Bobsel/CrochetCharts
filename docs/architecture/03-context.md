# 3. System Context and Scope

CrochetCharts is a single-user desktop application with no network services and no persistent state beyond user files. Its context is narrow.

## 3.1 Business Context

```mermaid
flowchart LR
    user([Crochet Designer])
    app[CrochetCharts]
    files[(.crochetcharts<br/>project files)]
    sets[(.set stitch sets<br/>+ SVG icons)]
    exports[(PDF / SVG / PNG<br/>exports)]
    printer([Printer])

    user -->|edits patterns| app
    app -->|save / load| files
    files -->|shared between machines| user
    app -->|import / export| sets
    sets -->|exchanged with<br/>other designers| user
    app -->|export| exports
    exports -->|publishing,<br/>printing, sharing| user
    app -->|print| printer
```

**Actors**
- **Crochet Designer** — sole user. Opens project files, composes charts, exports for publication.
- Other designers implicitly interact through shared `.set` files and project files.

**Produced / consumed artefacts**
- `.crochetcharts` — project file. See [08-crosscutting.md § Project file format](08-crosscutting.md#project-file-format).
- `.set` — stitch set. XML metadata + binary icon blob, see `src/stitchset.cpp`.
- PDF / SVG / PNG — export outputs, non-reversible.

## 3.2 Technical Context

```mermaid
flowchart TB
    subgraph host[Host Operating System]
        app[CrochetCharts<br/>native binary]
        qt[Qt 4.8 runtime<br/>QtGui · QtSvg · QtNetwork · QtXml]
        fs[(User filesystem)]
        print[CUPS / WinSpool / CoreGraphics]
        xorg[X11 / Wayland / WDM / Quartz]
        mime[XDG desktop / Launch Services]
    end

    app --> qt
    qt --> xorg
    qt --> print
    app --> fs
    app -. registers .-> mime
    mime -. opens .-> app

    settings[(QSettings store<br/>registry / ~/.config / plist)]
    qt --> settings
```

**Interfaces**

| Direction | Interface | Protocol / format |
|---|---|---|
| GUI display | OS windowing system | X11 via Qt4, Win32, Cocoa |
| Print | OS print subsystem | `QPrinter` over CUPS / WinSpool / Quartz |
| File open/save | OS filesystem | Binary QDataStream (`v1`) or XML + QDataStream framing (`v2`) |
| File association | Desktop / Launch Services | `vnd.stitchworks.pattern` MIME / UTI, `.crochetcharts` extension. See `resources/vnd.stitchworks.pattern.xml`. |
| Settings | OS-native store | `QSettings` — Windows registry / `~/.config` / macOS plist |
| Stitch assets | Qt resource system + user filesystem | 129 built-in SVGs via `:/stitches/` (see `stitches/stitches.qrc`); user-added sets under `Settings::instance().userPath()` |
| Update check (optional) | HTTPS | `src/updater.cpp` polls an HTTP endpoint; disabled by default |

**Out of scope**
- No server-side component.
- No collaboration / multi-user editing.
- No cloud storage integration.
- No telemetry / crash reporting.

## 3.3 External Dependencies

See also `.devcontainer/Dockerfile` for the exact build-time set.

| Dependency | Role | Version pin |
|---|---|---|
| Qt 4.8 | GUI, SVG, XML, network | `find_package(Qt4 REQUIRED)` — any 4.8.x |
| Hunspell | Optional spell-check (declared but currently **unused** in code) | `FindHunSpell.cmake`; `-DHUNSPELL_FOUND` |
| CMake | Build | ≥ 2.8.6 |
| xsltproc + FOP + docbook-xsl | End-user manual build (opt-in `-DDOCS=ON`) | Not required for app |
| Apple Developer certificates (mac) | Code signing for store/DMG | Required only for distribution |
| NSIS (Windows) | Installer generator | CPack-invoked |
