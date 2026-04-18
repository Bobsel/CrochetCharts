# stitches/ — SVG Stitch Library

129 `.svg` stitch icons + `stitches.qrc`. Qt resource prefix `/stitches` → access via `:/stitches/<name>.svg`.

## NAMING

Two coexisting conventions — do not normalize, do not rename existing files:

| Pattern | Meaning | Examples |
|---------|---------|----------|
| Crochet abbreviation | Standard stitch name | `sc.svg`, `hdc.svg`, `dc.svg`, `tr.svg`, `dtr.svg`, `ttr.svg`, `ch.svg`, `sl_st.svg`, `blo.svg`, `flo.svg` |
| Mixed case (preserved) | Front/back-post | `BPdc.svg`, `BPtr.svg`, `FPdc.svg`, `FPtr.svg` (case-sensitive — Linux filesystem, matters) |
| Compound | Multi-operation | `3dc_cluster.svg`, `5dc_popcorn.svg`, `sc2tog.svg`, `ch_close3tog_ch.svg` |
| Generic `stN` | User-added / numbered placeholders | `st1.svg` … `st77.svg` |
| Reserved names | System-used | `start.svg`, `end.svg`, `unknown.svg`, `loops.svg`, `ring.svg`, `adjustable_ring.svg`, `normal_closing.svg` |

## ADDING A NEW STITCH (checklist)

A new stitch needs **three synchronized edits**, otherwise it's invisible at runtime:

1. **SVG file**: drop `newstitch.svg` in this directory.
2. **`stitches.qrc`**: append `<file>newstitch.svg</file>` inside `<qresource prefix="/stitches">`. qrc is NOT globbed — manual entry required.
3. **`../crochet.xml`**: add a `<stitch>` block with `<name>`, `<icon>:/stitches/newstitch.svg</icon>`, `<description>`, `<category>`, `<ws>` (wrong-side abbreviation).

Skip any one and the stitch either fails to load (missing qrc entry → `:/stitches/...` returns nothing) or isn't pickable in the UI (missing xml entry).

## RULES

- **SVG viewBox**: existing stitches use a consistent cell-sized viewBox. New SVGs must match — use `sc.svg` or `dc.svg` as template for sizing/origin.
- **Colors**: leave stitches monochrome — the app re-colors via `colorreplacer`. Hardcoded colors will not tint correctly.
- **No text**: `svg <text>` elements may not render consistently with `QGraphicsSvgItem` in Qt 4.8.
- **Filename = id**: the filename (without `.svg`) is referenced in `crochet.xml` `<icon>` paths. Renaming breaks XML refs.
- **Filesystem case sensitivity**: `BPdc.svg` and `bpdc.svg` are different on Linux/macOS case-sensitive volumes. Preserve capitalization exactly.
- **Reserved names are load-bearing**: `unknown.svg` is the fallback icon; `start.svg`/`end.svg` mark pattern entry/exit. Do not remove or replace their semantics.

## BUILD WIRING

- Root `CMakeLists.txt` doesn't compile anything here.
- `src/CMakeLists.txt:6-11` adds `../stitches/stitches.qrc` to `crochet_resources` → embedded into the executable via `qt4_add_resources`.
- After editing `stitches.qrc`, a CMake re-run is NOT required — the Qt resource compiler tracks it, but a `make` rebuild of `qrc_*.cpp` is.
