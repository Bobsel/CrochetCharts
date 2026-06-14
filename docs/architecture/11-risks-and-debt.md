# 11. Risks and Technical Debt

Known, unresolved issues. Each entry: impact, likelihood, remediation sketch. Cross-referenced to the code.

## 11.1 Strategic risks

| # | Risk | Impact | Likelihood | Remediation |
|---|------|--------|------------|-------------|
| R-1 | **Qt 4 is EOL** and unbuildable on modern Linux distros. | High — future packaging and security updates are blocked. | Certain over time. | Full port to Qt 5 or 6 (ADR-01). Large effort. Not currently planned. |
| R-2 | **Test coverage ≈ 5 %.** | Medium — regressions slip through `Scene`, docks, export. | High whenever Scene/docks are touched. | Add tests per quality scenario (§ 10.2). Start with file round-trip, indicator undo, and mode transitions. |
| R-3 | **Linux-only CI.** `.github/workflows/build.yml` runs build + tests on every push / PR in the Bionic devcontainer (added 2026-06). Windows (MXE cross-compile) and macOS are still validated only by manual `task package:*` runs. | Medium — Windows / macOS regressions slip through; Linux is covered. | Medium on every cross-platform change. | Extend the workflow with a Windows job (reuse `.devcontainer/win/Dockerfile`) and a macOS job. |
| R-4 | **No macOS notarization, no Windows code-signing.** | Medium — end-user friction (Gatekeeper / SmartScreen). | Certain on fresh installs. | Extend `resources/installers.cmake` with notarize + signtool steps; provision certs. |
| R-5 | **Update channel** (`src/updater.cpp`) points at an HTTP endpoint; it has a known crash on cancel. | Low — updater is opt-in; most users never enable it. | Low. | Fix cancel path (see `updater.cpp:168`), switch to HTTPS, or remove updater altogether. |

## 11.2 Code-level debt

| # | Debt | Location | Severity | Notes |
|---|------|----------|----------|-------|
| D-1 | **`Scene` god class.** 3 626 lines, 142 methods, mode state machine, selection, undo dispatch, all in one. | `src/scene.cpp` | High | Do not add more logic here. Extract helper classes per mode (see [08-crosscutting.md § 8.5](08-crosscutting.md#85-mode-based-interaction)). |
| D-2 | **`MainWindow` secondary hub.** 2 043 lines, 116 methods, 32 includes. | `src/mainwindow.cpp` | Medium | Keep wiring minimal when adding docks/dialogs. Extract controllers if growing further. |
| D-3 | **Manual `new`/`delete` of `QString`.** | `file_v1.cpp:115`, `file_v2.cpp:108`, `stitchset.cpp:273,314` | Low | Replace with stack-allocated `QString`. Safe, mechanical. |
| D-4 | **`QMimeData` leak in `colorlistwidget`.** | `colorlistwidget.cpp:64–65` | Low | `setMimeData` on a drag transfers ownership; the local `new` is leaked on the early-return path. |
| D-5 | **`QSvgRenderer` without parent ownership** in cell rendering. | `cell.cpp` — check constructor | Low | Verify; if heap-allocated without parent, switch to member-by-value or set parent. |
| D-6 | ~~**Hardcoded Windows paths** `C://qt//4.8.5//bin`, `C://MinGW-4.4//bin`.~~ **Resolved 2026-04** — replaced by `BUNDLE_QT_DLLS` option (default ON on native Windows, OFF when `CMAKE_CROSSCOMPILING`) + `MINGW_BIN_DIR` cache variable. See `src/CMakeLists.txt:22-48`. | `src/CMakeLists.txt` | Resolved | Do not reintroduce hardcoded paths. |
| D-7 | **No `Q_DISABLE_COPY`.** Model classes are accidentally copyable. | across `src/` | Low | Add `Q_DISABLE_COPY(ClassName)` to all `QObject`-derived classes that should not be copied. |
| D-8 | **`QGraphicsItem::Type` collision risk.** Custom `UserType + N` values scattered without central enum. | `cell.h`, `indicator.h`, `ChartImage.h`, `guideline.h`, `itemgroup.h` | Low | Centralise to a single enum in a shared header. |
| D-9 | **Singleton shutdown order is implicit.** Using `Settings::inst()` during `QApplication` teardown can crash. | singleton users in destructors | Low | Audit destructors of long-lived objects. None known to hit this today. |
| D-10 | **`//TODO transform_refactor` markers** signal an incomplete effort. | grep `scene.cpp` | Low | Inherit the intent when editing nearby code; do not layer on. |
| D-11 | **Known FIXMEs worth reading** before touching: | | | |
|      | "crashes when removing sets with sts in the master list" | `stitchlibraryui.cpp:590` | High locally | |
|      | Ctrl-selection fix with unknown side effects | `scene.cpp:540` | Medium | |
|      | `fileSave` cancel still closes the window | `mainwindow.cpp:955` | Medium | |
|      | Crash on updater cancel | `updater.cpp:168` | Low (updater opt-in) | |
| D-12 | **No `.clang-format` / `.clang-tidy`.** Style drifts between contributors. | repo root | Low | Add configs mirroring the dominant existing style. Run once, commit, then require clean for new changes. |
| D-13 | **Hunspell declared but unused.** `find_package` and include path exist; no call site. | `src/CMakeLists.txt`, possibly `src/mainwindow.cpp` | Low | Either wire spell-check into text items or remove the dependency. |
| D-14 | **Scene friend surface** — serializers reach into many private fields. | see ADR-06 | Medium | Acknowledged cost. Keep only if serialization tests grow to actually catch mistakes. |
| D-15 | **No plug-in path for user stitches requiring a rebuild of the binary.** | `stitches/stitches.qrc` | Low | By design (ADR-07). `.set` files compensate. |

## 11.3 Debt remediation priorities

Sorted by expected payoff per hour of work:

1. **D-3 + D-4** — trivial, mechanical fixes. Half-day. Eliminates known leaks.
2. **R-2 / Quality scenarios** — add file round-trip and undo symmetry tests. Multi-day but high leverage.
3. **D-11 known FIXMEs** — targeted bug fixes in code the user already hits.
4. **R-3 / Windows + macOS CI** — Linux done (`.github/workflows/build.yml`); extend with a Windows MXE job and a macOS job to catch cross-platform regressions.
5. **D-1 / `Scene` extraction** — ongoing. Extract one mode at a time into a helper; don't attempt a grand refactor.

Everything below that point is low impact relative to the cost of getting Qt 4 to keep working at all (R-1).
