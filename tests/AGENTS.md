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
- **Data-driven tests**: `foo()` + `foo_data()`. Add columns with `QTest::addColumn<T>()`, rows with `QTest::newRow()`, fetch with `QFETCH(T, name)`. SHA1 hashing used in `teststitch.cpp` for image comparison — follow that pattern for graphics tests.
- **Fixtures**: member variables + `initTestCase()` / `cleanupTestCase()`. No per-test `init()`/`cleanup()` currently.
- **No mocks / no gmock**. Uses real `QGraphicsScene`, `QSvgRenderer`. If mocking is needed, fake at the Qt level (subclass + override).
- **Registration is manual**: new test class → add `#include "testfoo.h"` and a `new TestFoo()` / `QTest::qExec` / `delete` block in `tests/main.cpp` (NOT auto-registered).
- **`QTEST_MAIN` is NOT used** (it would define its own main; incompatible with the chain-all-tests approach).

## BUILD MODEL (unusual — read this)

`tests/CMakeLists.txt` does **not** link a library. It compiles the test `.cpp` files **alongside an explicit list of `../src/*.cpp`** (variable `crochet_app_cpp`, ~43 files) into a single `tests` executable. Same `.qrc` and `.ui` files as main app.

Consequences:
- **Adding a new `src/*.cpp`**: edit `tests/CMakeLists.txt` and append to `crochet_app_cpp`, or tests fail to link.
- **Test binary pulls in QtGui/QtSvg/QtNetwork** — tests must have a running `QApplication` (see `main.cpp:30`).
- Build adds ~1 minute; CTest is not wired — tests run by executing the binary directly.

## COMMANDS

```bash
task test                  # preferred: cmake -DUNIT_TESTING=ON + make + run
./bin/tests                # legacy wrapper
cd build && ctest          # NOT configured — does nothing useful
```

## ANTI-PATTERNS

- **Test order dependency**: `TestSettings` runs first by design (comment: "dep: license (initDemo)"). Do not reorder blindly in `main.cpp`.
- **Hardcoded stitch names** in `teststitch.cpp:39` (`FIXME: don't hard code the stitches`) — new stitches may break these tests without touching code.
- **No negative / failure-path tests** — happy-path only across the suite.
- **SHA1-based image assertions** (`teststitch.cpp` `stitchRender_data`) are fragile to Qt/SVG rendering changes.
