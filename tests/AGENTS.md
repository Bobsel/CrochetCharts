# tests/ — Unit Tests

Qt 4 `QtTest`-based unit tests. 8 test classes, covers ~5% of src/. Not a safety net — with two exceptions: `TestFileFactory` and `TestChartView` are the Qt4→Qt6 port regression oracles.

## STRUCTURE

```
main.cpp                       # Manual QTest::qExec() chain — ORDER MATTERS (TestSettings runs first)
test{name}.{cpp,h}             # One class per module (TestSettings/Stitch/StitchSet/StitchLibrary/Cell/TextView/FileFactory/ChartView)
CMakeLists.txt                 # Builds `tests` executable — RELINKS the whole app; wires CTest with xvfb-run
fixtures/                      # Versioned test inputs (decoupled from production content)
fixtures/port/                 # Frozen Qt4-era fixtures + snapshots — see fixtures/port/README.md
```

## WHAT'S COVERED vs NOT

| Covered | Not covered (major gaps) |
|---------|--------------------------|
| stitch, stitchset, stitchlibrary, cell, settings, textview, filefactory (v2 XML + v1 binary round-trip), chartview (zoom/wheel semantics only) | scene, crochettab, all docks, all dialogs, crochetchartcommands, exportui, ChartImage, colorreplacer, legends, indicators, itemgroup, chartLayer, mainwindow (beyond construction smoke), debug |

## RULES

- **Framework**: `QtTest` only. `#include <QtTest/QTest>`. Macros: `QVERIFY`, `QCOMPARE`, `QFETCH`, `QFAIL`, `QWARN`. **No gtest, no Catch2** — do not introduce.
- **Class naming**: `Test<Module>` in `test<module>.{h,cpp}`. Test methods live under `private slots:` — that's what QtTest enumerates.
- **Data-driven tests**: `foo()` + `foo_data()`. Add columns with `QTest::addColumn<T>()`, rows with `QTest::newRow()`, fetch with `QFETCH(T, name)`. Column count and type must match exactly (QtTest QFATALs on type mismatch and aborts the process — killing all subsequent test classes).
- **Prefer semantic assertions over pixel comparisons**: assert `c->rotation()`, `ChartItemTools::getScaleX(c)`, `c->bgColor()`, `s->isValid()`, pixel-count > 0 — *not* SHA1 of rendered PNGs/SVGs. The latter breaks on every Qt / fontconfig / SVG-engine update.
- **Assertion-free slots are banned**: an empty `private slot` body reports PASS and lies about coverage. Every slot must QVERIFY / QCOMPARE at least one thing or be deleted.
- **Fixtures**: member variables + `initTestCase()` / `cleanupTestCase()`. No per-test `init()`/`cleanup()` currently.
- **No mocks / no gmock**. Uses real `QGraphicsScene`, `QSvgRenderer`. If mocking is needed, fake at the Qt level (subclass + override).
- **Registration is manual**: new test class → add `#include "testfoo.h"` and a `new TestFoo()` / `QTest::qExec` / `delete` block in `tests/main.cpp` (NOT auto-registered).
- **`QTEST_MAIN` is NOT used** (it would define its own main; incompatible with the chain-all-tests approach).

## BUILD MODEL (unusual — read this)

`tests/CMakeLists.txt` does **not** link a library. It compiles the test `.cpp` files **alongside an explicit list of `../src/*.cpp`** (variable `crochet_app_cpp`, ~48 files — src/ uses `file(GLOB)` but the tests target does not) into a single `tests` executable. Same `.qrc` and `.ui` files as main app.

Consequences:
- **Adding a new `src/*.cpp`**: edit `tests/CMakeLists.txt` and append to `crochet_app_cpp`, or tests fail to link.
- **Test binary pulls in QtGui/QtSvg/QtNetwork** — tests must have a running `QApplication` (see `main.cpp:30`) and an X display. `task test` installs/uses `xvfb-run` inside the devcontainer; on hosts with a real `DISPLAY` it falls through.
- **CWD must be `tests/`** when running the binary. Inputs resolve via `../crochet.xml`, `../stitches/*.svg`, and `fixtures/...`. `task test` runs from `tests/` automatically; if invoking the binary by hand, `cd tests && ../build/tests/tests`.
- Build adds ~1 minute; CTest is not wired — tests run by executing the binary directly.

## COMMANDS

```bash
task test                              # preferred: cmake -DUNIT_TESTING=ON + make + run
./bin/tests                            # legacy wrapper
cd build && ctest --output-on-failure  # CI-style runner; same binary, same CWD, xvfb-run auto-detected
```

## FIXTURES

`tests/fixtures/` holds versioned test inputs decoupled from production content.
- `basic_stitches.xml` — 5 stitches (sl st, ch, sc, hdc, dc) under category `Test`. Used by `TestStitchSet` so that edits to `crochet.xml` do not cascade into test failures.
- `fixtures/port/` — **frozen Qt4-era port fixtures** for both save formats:
  - v2 XML format: `blank_v2.crochetcharts`, `basic_v2.crochetcharts`, `basic_v2.snapshot.txt`
  - v1 binary format (`QDataStream::Qt_4_7`): `legacy_v1.crochetcharts`, `legacy_v1.snapshot.txt`

  Generated once on the Qt4 baseline; committed as bytes. The Qt6 port's regression oracle for the file I/O layer. See `fixtures/port/README.md`. **Never regenerate on CI.**
- Prefer adding new fixtures here over loading `../crochet.xml`.

## PORT-REGRESSION SUITE (`TestFileFactory`, `TestChartView`)

Purpose: provide a Qt-version-independent semantic oracle for the two highest-risk parts of the Qt4→Qt6 port — the file I/O layer (both serializers) and the `QWheelEvent::delta()` zoom bridge.

### `TestFileFactory` — file I/O oracle

- **Dump format** (`dumpScene(scene)` in `testfilefactory.cpp`): one line per `Cell` item, fields `pos rot scale bg color stitch layer`, floats to 3 decimals, lexicographically sorted for determinism. Extend the dump fields in the same commit as the field being tracked.
- **Generate-if-missing pattern**: `generate*_ifMissing()` slots write the fixture/snapshot when absent, otherwise assert non-empty. First run seeds; subsequent runs verify. After first seeding, **commit the files** — CI must never see a missing fixture.
- **Both file formats are pinned**: v2 XML (`Version_1_2`) via `basic_v2`/`blank_v2` fixtures, v1 binary (`Version_1_0`, `QDataStream::Qt_4_7`) via `legacy_v1`. Round-trip tests (`roundTrip_*`) save-then-reload a loaded fixture through a `QTemporaryFile` and assert dump equality; this catches any serialization drift between Qt4 and the target Qt version.
- **`TestMW` helper** (anonymous namespace of `testfilefactory.cpp`): subclasses `MainWindow` to expose `protected: tabWidget()` and `createTab()` via `using`-declarations. Needed because tests are not friends of `MainWindow`. Use this pattern for future tests that need to drive `MainWindow` programmatically.

### `TestChartView` — wheel/zoom oracle

- **Targets the Qt6 `QWheelEvent::delta()` removal** (`chartview.cpp:109`). Two test groups: `zoom_*` exercises the public `zoom(int)` arithmetic and is Qt-version-independent; `wheelEvent_*` exercises the protected `wheelEvent(QWheelEvent*)` handler via synthetic events and will fail to compile once `QWheelEvent::delta()` is removed — the intended signal to port this call site to `angleDelta().y()`.
- **`TestCV` helper** (anonymous namespace of `testchartview.cpp`): subclasses `ChartView` to expose `protected: wheelEvent` via a `using`-declaration. Do NOT switch to `QApplication::sendEvent(view->viewport(), &e)` — that couples the test to `QGraphicsView` viewport event-filter internals which are incidental.
- Observables are `transform().m11()` and the `zoomLevelChanged(int)` signal (via `QSignalSpy`). No rendering, no pixels.

### Noise to ignore

- **`QPainter::fontMetrics: Painter not active` warnings** during `TestFileFactory` runs are benign Qt4 noise — widgets are constructed without being shown. Ignore unless they become errors.
- **`loadChart Unknown tag: ""`** warnings are a pre-existing quirk in `File_v2::loadChart` (whitespace between XML elements). Not test-introduced.

## ANTI-PATTERNS

- **Test order dependency**: `TestSettings` runs first by design (comment: "dep: license (initDemo)"). Do not reorder blindly in `main.cpp`.
- **Coupling to production content**: loading `../crochet.xml` and hardcoding a stitch count / file path couples the test to every content change. Use `tests/fixtures/*.xml` instead. (`teststitch.cpp` still loads `../stitches/ch.svg` etc. for basic stitches; that list is stable enough to be acceptable.)
- **No negative / failure-path tests** — happy-path only across the suite.
- **`StitchLibrary::findStitch` never returns null** — when a name is absent it synthesises a placeholder with `file = ":/stitches/unknown.svg"` (see `stitchlibrary.cpp:235`). Do not write assertions expecting `nullptr` for unknown names; assert the placeholder contract instead.
- **Pixel/SHA1 image assertions** are banned (historically used in `testcell.cpp` and `teststitch.cpp`). They break on any rendering-layer change and tell you nothing useful when they do.
