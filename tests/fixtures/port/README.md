# Qt4 → Qt6 Port Fixtures

Frozen-behavior inputs for the port regression suite. These files must remain
byte-identical from the moment they are committed — they are the oracle
against which Qt5.15 and Qt6 builds are judged.

## Contents (planned)

| Fixture | Purpose |
|---------|---------|
| `blank_v2.crochetcharts` | Empty chart — smallest valid v2 file. Smoke-tests that the loader starts. |
| `basic_v2.crochetcharts` | 3–5 stitches (ch/sc/hdc/dc). Exercises QString + QColor + QTransform round-trip. |
| `legacy_v1.crochetcharts` | Old binary QDataStream format. Covers the AGENTS.md "not stable across Qt versions" warning. |
| `layers_groups_v2.crochetcharts` | Multiple layers + grouped items. Exercises File_v2 friend-class intrusion into Scene/Cell/ItemGroup. |
| `colors_v2.crochetcharts` | Heavy palette usage. Isolates QColor (de)serialization drift. |

## Generation

Fixtures will be produced by a one-shot helper in `testfilefactory.cpp`
that constructs a minimal in-memory Scene via the app code and writes it
through `FileFactory::save()`. The generator only runs when the file is
missing; committed fixtures are never regenerated.

## Snapshots

Each `*.crochetcharts` fixture will be paired with a `*.snapshot.txt`
canonical dump of its loaded scene state (one line per item, sorted,
floats rounded to 3 decimals). The snapshot is the regression oracle.
Byte-equality of exported files is NOT required and NOT tested.
