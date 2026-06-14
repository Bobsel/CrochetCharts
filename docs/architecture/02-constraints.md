# 2. Architecture Constraints

Hard boundaries that cannot be relaxed without a major rewrite.

## 2.1 Technical Constraints

| Constraint | Source | Impact |
|---|---|---|
| **Qt 4.8** | Historical; see ADR-01 in [09-decisions.md](09-decisions.md) | No Qt5/Qt6 APIs. SIGNAL/SLOT string macros, `qInstallMsgHandler`, `qt4_wrap_ui`. Qt4 is EOL and unsupported upstream. |
| **C++11 (`-std=gnu++11`)** | `src/CMakeLists.txt` | No C++14/17/20. No `std::optional`, no structured bindings, no `std::filesystem`. |
| **CMake ≥ 2.8.6** | Root `CMakeLists.txt` | Modern CMake idioms (targets, generator expressions) are partially usable but project style predates them. |
| **Single native binary per platform** | Distribution model | No plugin host, no embedded scripting, no separate worker process. |
| **Bind-mounted dev container on Ubuntu 18.04** | `.devcontainer/Dockerfile` | Only Bionic-era packages, mitigated by PPAs (git-core for current git). Base OS is EOL. See [07-deployment.md](07-deployment.md). |
| **SVG as the only stitch icon format** | `src/cell.cpp`, `stitches/*.svg` | Raster icons, vector fonts, or font-as-icon schemes are out of scope. |

## 2.2 Organisational Constraints

| Constraint | Source | Impact |
|---|---|---|
| **GPLv3 code, CC BY-SA 4.0 art** | `LICENSE`, `README.md` | Derivative works must be GPL-compatible. Bundled libraries must be compatible or dynamically linked. |
| **Feature freeze; maintenance mode** | Git history (sparse since 2014) | Large refactors without user-visible benefit are discouraged. |
| **No CI infrastructure** | Absence of `.github/`, `.gitlab/` | Validation is manual via `task test`. See [07-deployment.md](07-deployment.md). |

## 2.3 Conventions

These are project-local constraints — see also `AGENTS.md` § Conventions.

- **Flat `src/`** — all 99 `.cpp/.h` and 14 `.ui` files live directly under `src/`. No subdirectories by design.
- **Member prefix `m`** — `mHighlight`, `mType`. Applies to all new members.
- **Singletons via `inst()` static** — `Settings::inst()`, `AppInfo::inst()`, `StitchLibrary::inst()`. Not thread-safe; accessed only from the GUI thread.
- **Versioned file format** — new formats added as `file_v3.{cpp,h}`; `v1` and `v2` are frozen. `FileFactory` dispatches.
- **Friend class for I/O** — `Cell`, `Scene`, `MainWindow`, `CrochetTab`, `Indicator`, `StitchSet`, `StitchLibrary`, `TabInterface` (and `FileFactory` itself) declare `friend class File_v1/v2/FileFactory/SaveFile` (per class, the subset that actually reads/writes it). Encapsulation is intentionally pierced by the serializers. See [08-crosscutting.md § Serialization](08-crosscutting.md#friend-based-serialization).
- **`Q_DISABLE_COPY` unused** — copyability of model classes is accidental and should not be relied upon.
- **No `.clang-format` / `.editorconfig`** — style is inherited from surrounding code.
