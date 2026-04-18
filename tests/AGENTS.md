# tests/ — Unit Tests

Qt 4 `QtTest`-based unit tests. 6 test classes, covers ~5% of src/. Not a safety net.

## STRUCTURE

```
main.cpp                       # Manual QTest::qExec() chain — ORDER MATTERS (TestSettings runs first)
test{name}.{cpp,h}             # One class per module (TestSettings/Stitch/StitchSet/StitchLibrary/Cell/TextView)
CMakeLists.txt                 # Builds `tests` executable — RELINKS the whole app
```

## WHAT'S COVERED vs NOT

| Covered | Not covered (major gaps) |
|---------|--------------------------|
| stitch, stitchset, stitchlibrary, cell, settings, textview | scene, chartview, crochettab, all docks, all dialogs, crochetchartcommands, file/file_v1/file_v2/filefactory, exportui, ChartImage, colorreplacer, legends, mainwindow, debug |

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
task test                  # preferred: cmake -DUNIT_TESTING=ON + make + run
./bin/tests                # legacy wrapper
cd build && ctest          # NOT configured — does nothing useful
```

## FIXTURES

`tests/fixtures/` holds versioned test inputs decoupled from production content.
- `basic_stitches.xml` — 5 stitches (sl st, ch, sc, hdc, dc) under category `Test`. Used by `TestStitchSet` so that edits to `crochet.xml` do not cascade into test failures.
- Prefer adding new fixtures here over loading `../crochet.xml`.

## ANTI-PATTERNS

- **Test order dependency**: `TestSettings` runs first by design (comment: "dep: license (initDemo)"). Do not reorder blindly in `main.cpp`.
- **Coupling to production content**: loading `../crochet.xml` and hardcoding a stitch count / file path couples the test to every content change. Use `tests/fixtures/*.xml` instead. (`teststitch.cpp` still loads `../stitches/ch.svg` etc. for basic stitches; that list is stable enough to be acceptable.)
- **No negative / failure-path tests** — happy-path only across the suite.
- **`StitchLibrary::findStitch` never returns null** — when a name is absent it synthesises a placeholder with `file = ":/stitches/unknown.svg"` (see `stitchlibrary.cpp:235`). Do not write assertions expecting `nullptr` for unknown names; assert the placeholder contract instead.
- **Pixel/SHA1 image assertions** are banned (historically used in `testcell.cpp` and `teststitch.cpp`). They break on any rendering-layer change and tell you nothing useful when they do.
