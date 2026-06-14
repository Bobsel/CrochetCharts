# Qt4 → Qt6 Port Fixtures

Frozen-behavior inputs for the port regression suite. These files must remain
byte-identical from the moment they are committed — they are the oracle
against which Qt5.15 and Qt6 builds are judged.

## Contents

### Present

| Fixture | Format | Purpose |
|---------|--------|---------|
| `blank_v2.crochetcharts` | v2 XML (`Version_1_2`) | Empty chart — smallest valid v2 file. Smoke-tests that the loader starts. |
| `basic_v2.crochetcharts` | v2 XML (`Version_1_2`) | 3 `ch` stitches. Exercises QString + QColor + QTransform round-trip. |
| `legacy_v1.crochetcharts` | v1 binary (`Version_1_0`, `QDataStream::Qt_4_7`) | Same chart as `basic_v2` but serialized through `File_v1`. Pins the binary format + embedded stitch-icon blob against Qt version drift. |

Each `*.crochetcharts` is paired with a `*.snapshot.txt` — the canonical
dump of its loaded scene state (`dumpScene()` in `testfilefactory.cpp`),
one line per `Cell`, sorted, floats to 3 decimals. Snapshots are the
regression oracle; byte-equality of exported files is NOT required and
NOT tested.

### Future candidates (not yet seeded)

| Fixture | Purpose |
|---------|---------|
| `layers_groups_v2.crochetcharts` | Multiple layers + grouped items + indicators. Requires extending `dumpScene` to cover `Indicator`, `ItemGroup`, and per-layer `ChartLayer` metadata. |
| `colors_v2.crochetcharts` | Heavy palette usage. Isolates QColor (de)serialization drift. |
| `custom_stitches_v2.crochetcharts` | Exercises `<stitch_set>` XML block in `File_v2` and `saveIcons`/`loadIcons` in `File_v1`. |

## Generation

Fixtures are produced by `generate*_ifMissing()` slots in
`testfilefactory.cpp` that construct a minimal in-memory Scene via the
app code and write it through `FileFactory::save()`. The generator only
runs when the file is missing; committed fixtures are never regenerated.
