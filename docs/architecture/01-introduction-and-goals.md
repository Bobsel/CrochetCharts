# 1. Introduction and Goals

## 1.1 Purpose

CrochetCharts is a **desktop editor for crochet charts**: a WYSIWYG canvas for placing, arranging, colouring, and exporting crochet stitch diagrams. Users import or compose patterns stitch-by-stitch, arrange them on a 2D canvas (round or flat layouts), and export as PDF, SVG, or PNG for print or web.

The software is the reference implementation published by Stitch Works Software, GPLv3, with artwork under CC BY-SA 4.0.

## 1.2 Essential Features

| Feature | Where it lives |
|---|---|
| Chart canvas with selection, rotation, scaling, layer ordering | `src/scene.cpp`, `src/chartview.cpp` |
| Stitch placement using a library of SVG icons | `src/scene.cpp` + `src/stitchlibrary.cpp` |
| Custom stitch sets (user-extendable SVG + metadata) | `src/stitchset.cpp`, `src/stitchlibrary.cpp` |
| Multiple charts per document (tabs) | `src/mainwindow.cpp` + `src/crochettab.cpp` |
| Undo/redo | `src/crochetchartcommands.cpp` (18+ QUndoCommand subclasses) |
| Versioned binary/XML project file | `src/file.h` + `file_v1.cpp` + `file_v2.cpp` + `filefactory.cpp` |
| Export to PDF/SVG/PNG | `src/exportui.cpp` |
| Cross-platform packaging (Win/macOS/Linux) | `resources/installers.cmake` + CPack |

## 1.3 Quality Goals

In decreasing priority. Higher goals override lower ones when they conflict.

| # | Goal | Rationale |
|---|------|-----------|
| 1 | **Correctness of saved files** | Users lose hours of pattern design if a save/load round-trip corrupts data. Backwards compatibility with `v1` files (pre-XML) is mandatory. |
| 2 | **Interactive responsiveness on a hobbyist laptop** | Target user is a crochet designer, not an engineer. Canvas manipulation must feel immediate for charts of ~500 stitches. |
| 3 | **Cross-platform parity** | Windows, macOS, Linux must produce identical output files and near-identical UI. |
| 4 | **Installability by non-technical users** | One-click installers per platform, no command line. |
| 5 | **Extensibility of stitch catalog** | Designers must be able to add custom stitch sets (SVG + XML) without rebuilding. |
| 6 | **Maintainability of the C++ codebase** | Eroded — see [11-risks-and-debt.md](11-risks-and-debt.md). Acknowledged but ranked last because the project is feature-complete and bug-fix-only. |

## 1.4 Stakeholders

| Role | Expectation |
|---|---|
| End-user (crochet designer) | Ships on their OS, opens their old files, exports print-quality charts. |
| Maintainer(s) | Can fix bugs and add stitches without deep Qt4 archaeology. |
| Packager (distro, store) | CPack produces a valid installer per platform. |
| Contributor of SVG stitches | Can add an SVG + `crochet.xml` entry and have it appear in the stitch picker. |

There is no commercial operator, no SLA, no telemetry. Distribution is self-service download.
