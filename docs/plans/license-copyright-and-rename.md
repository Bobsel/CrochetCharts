# License, Copyright, and Rename Plan

**Status:** Draft
**Last updated:** 2026-04-18
**Scope:** Document the current licensing/copyright state of this fork and define
a plan + guideline for continuing development under a different name.

This document has two halves:

1. **Findings** — what the repository currently declares and where.
2. **Plan & guideline** — what to do (and not do) if the project is renamed and
   continued by a new maintainer.

---

## 1. Findings

### 1.1 Upstream provenance

- **Upstream:** `https://github.com/StitchworksSoftware/CrochetCharts` (archived).
- **Original primary author:** Brian C. Milco (`bcmilco@gmail.com`, a.k.a.
  *iPenguin*).
- **Origin years:** 2010-2014 (per source headers).
- **This fork:** `https://github.com/Bobsel/CrochetCharts.git`
  (`git remote -v`, branch `upgrade`).
- **History:** full upstream history preserved. First commit
  `67d2a84 2010-11-19 inital commit with basic ui and classes.` is intact —
  important for GPLv3 compliance.

### 1.2 Declared licensing

| Artifact | Declaration | Evidence |
|---|---|---|
| `LICENSE` | GNU GPL v3 verbatim (FSF, 29 June 2007) | `LICENSE:1-7` |
| `README.md` | "GPLv3. All original artwork … cc by-sa 4.0" | `README.md:5` |
| `AGENTS.md` (root) | "GPLv3 / CC BY-SA 4.0 artwork" | `AGENTS.md:9` |
| Source file headers | `Copyright (c) 2010-2014 Stitch Works Software / Brian C. Milco <bcmilco@gmail.com>` + full GPLv3 grant ("either version 3 … or any later version") | e.g. `src/main.cpp:2-20` (pattern repeats in 41+ files) |
| About dialog | Embeds a copyright string | `src/mainwindow.cpp:880` |

Implication: the effective SPDX identifier is **`GPL-3.0-or-later`** (because
every file grants "version 3 … or any later version"). Artwork claim is
**`CC-BY-SA-4.0`**.

### 1.3 Gaps and issues found

1. **CC BY-SA 4.0 text is missing on disk.** The license is asserted in
   `README.md:5` and `AGENTS.md:9` but there is no `LICENSES/CC-BY-SA-4.0.txt`
   or equivalent. CC BY-SA 4.0 requires the license text (or a clear link) and
   attribution to travel with the work.
2. **No fork-level copyright line.** Substantive changes have been made (arc42
   docs, devcontainer, Taskfile, test suite refactor, v1 binary regression
   fixtures, etc.) but no file carries a second copyright line for the current
   maintainer, and there is no `AUTHORS` / `CONTRIBUTORS` / `NOTICE` file.
   GPLv3 §5a requires modified files to "carry prominent notices stating that
   you modified it, and giving a relevant date."
3. **Typo in `src/scene.h:2`:** `C opyright (c) 2010-2014 …` (stray space).
   Cosmetic, pre-existing upstream.
4. **No SPDX identifiers.** Not required, but adding
   `SPDX-License-Identifier: GPL-3.0-or-later` simplifies tooling and review.
5. **`README.md` doesn't state this is a fork.** It neither names the original
   author nor links to the archived upstream.
6. **About dialog string (`src/mainwindow.cpp:880`)** embeds a runtime
   copyright message — it will stay stale unless explicitly updated.
7. **Third-party files in-tree** (license notices must be preserved):
   - `cmake/modules/GetGitRevisionDescription*` — typically Boost-licensed.
   - `bin/gprof2dot.py` — LGPL.
   - `bin/asan_symbolize.py` — NCSA/Apache-2.0-ish (LLVM).
   All are GPL-compatible; do not scrub their headers during any rename sweep.

### 1.4 What is actually fine

- GPLv3 is correctly shipped in full as `LICENSE`.
- Every inspected source file carries the original copyright line and the full
  GPLv3 grant paragraph.
- Git history is intact — no squash that would lose upstream commits.
- Remote points at the maintainer's personal fork, which is acceptable.

---

## 2. Continuing under a different name — plan & guideline

Rebranding a GPLv3 project is permitted. There are four distinct concern
categories; treat them separately.

### 2.1 License obligations (GPLv3 §5 / §7)

**Must do:**

- Keep every existing `Copyright (c) 2010-2014 Stitch Works Software /
  Brian C. Milco` line. Add, never replace.
- Add a second copyright line for the new maintainer and a "modified from
  CrochetCharts" notice with a date to every file you meaningfully change
  (GPLv3 §5a). Suggested header block:

  ```text
  Copyright (c) 2010-2014 Stitch Works Software
  Copyright (c) 2026 <New Maintainer>
  Modified from CrochetCharts; renamed to <NewName> in 2026.
  ```

- Keep the GPLv3 grant paragraph in every source file.
- Keep the top-level `LICENSE` (GPLv3).
- Stay GPL-compatible. The existing grant is
  `GPL-3.0-or-later`; either keep that or narrow to `GPL-3.0-only`. You
  **cannot** relicense to MIT / Apache / proprietary — original authors hold
  copyright on their portions.
- Preserve third-party notices in `bin/gprof2dot.py`,
  `bin/asan_symbolize.py`, `cmake/modules/GetGitRevisionDescription*`.

**Must not do:**

- Delete or overwrite the original copyright notices.
- Use `sed` to remove "Stitch Works Software" or "Brian C. Milco" from file
  headers.
- Claim sole authorship of the codebase.

### 2.2 Trademark / naming

GPLv3 covers copyright, not trademarks.

- **Pick a clearly distinct name.** Avoid "CrochetCharts 2", "CrochetCharts
  NG", "Crochet Charts Plus" — anything a user could mistake for the upstream.
- **Avoid using Stitch Works Software wordmarks / logos** in the UI or in
  release artifacts (icons, splash, website).
- **Attribute, don't imply endorsement.** A line like "Based on CrochetCharts
  by Brian C. Milco / Stitch Works Software" in README and About is
  appropriate. Do not say the original authors maintain, endorse, or are
  affiliated with the fork.

### 2.3 Attribution hygiene

Add before the rename, as separate commits:

- `AUTHORS` (or `CONTRIBUTORS`) listing the original author(s) and current
  maintainer(s).
- `NOTICE` or a README section:

  ```text
  This project is a fork of CrochetCharts
  (https://github.com/StitchworksSoftware/CrochetCharts, archived).
  Original copyright © 2010-2014 Brian C. Milco / Stitch Works Software.
  Renamed to <NewName> and continued by <New Maintainer>, 2026-.
  ```

- Preserve git history on the first push (no squash-rebase that drops upstream
  commits).

### 2.4 Artwork & assets (CC BY-SA 4.0)

The stitch SVGs (`stitches/*.svg`) and toolbar icons (`images/`) carry the
artwork license claim. Renaming does not weaken these obligations.

- Ship `LICENSES/CC-BY-SA-4.0.txt` (canonical text from
  `https://creativecommons.org/licenses/by-sa/4.0/legalcode.txt`).
- Add an `ARTWORK.md` (or a README section) naming **Stitch Works Software**
  as the attribution target for stitches and images, with a link to the
  license and a note that the artwork has been modified (if/when it is).
- New artwork contributed by the new maintainer should be released under
  CC BY-SA 4.0 (compatible) or another ShareAlike-compatible license. Do not
  mix in CC-BY-NC or proprietary artwork.
- Replace app-identity artwork (splash, app icon, macOS
  `images/Crochet Charts.iconset/`, document icon
  `images/PatternDocument.iconset/`). These are what users recognize as the
  trademark; leaving them intact under a new name is the main trademark risk.

---

## 3. Mechanical rename checklist

Do not run `sed -i 's/CrochetCharts/NewName/g' **` blindly. The rename touches
four layers: user-visible strings, build/packaging identifiers, on-disk
file-format compatibility, and tests. Work through them in the order below.

### 3.1 User-visible strings

- `src/appinfo.cpp`, `src/appinfo.h` — `AppName`, `AppOrg`, `AppDomain`.
  Changing these changes the Qt `QSettings` key namespace; existing user
  preferences will become invisible unless migrated.
- `src/mainwindow.cpp:880` — About-dialog copyright string.
- `src/splashscreen.{cpp,h}` and the splash image referenced from
  `crochet.qrc`.
- `README.md`, `AGENTS.md` (root + `src/AGENTS.md` + `tests/AGENTS.md` +
  `stitches/AGENTS.md`), `.agents/` skills, `docs/architecture/**`,
  `docs/index.docbook.in`.

### 3.2 Build / packaging identifiers

- `CMakeLists.txt` — `project(CrochetCharts …)`, target name in
  `src/CMakeLists.txt`.
- `crochet.qrc`, `crochet.rc`, `crochet.xml` — filenames and RCC prefixes.
- `resources/*.desktop.in`, MIME XML, NSIS template under
  `cmake/modules/`, Debian `postinst`/`prerm`, macOS `Entitlements.plist`.
- CPack variables: `CPACK_PACKAGE_NAME`, `CPACK_PACKAGE_VENDOR`,
  `CPACK_NSIS_*`, macOS bundle identifier (e.g.
  `com.stitchworkssoftware.*` → `com.<newdomain>.*`).
- Windows icon resource (`crochet.rc`), macOS `Info.plist` template.
- `Taskfile.yml` targets that reference the binary name or build dirs.
- `.devcontainer/` image tags (if any).

### 3.3 File-format compatibility

- `src/file_v1.cpp` / `src/file_v2.cpp` — check for magic strings or XML root
  identifiers naming the app. Strategy:
  - On **read**: accept both old and new identifiers.
  - On **write**: emit the new identifier (or, for maximum compat, keep the
    old one and advertise compatibility).
- `resources/*.xml` (MIME): decide whether to keep the old MIME for
  back-compat and add a new one, or keep only the old. Keeping both is
  friendliest to existing users.
- `QSettings` organization/application: if you rename, either migrate old
  settings on first run or document that users will see defaults.

### 3.4 Repository and remotes

- Rename the GitHub repo (GitHub redirects the old URL; still update README
  links and badges).
- Update the local `origin` URL (`git remote set-url origin …`).
- Re-tag a release after the rename so `git describe --tags --dirty=w`
  (used by `cmake/modules/version.cpp.in`) produces a clean version string
  for the new name.

### 3.5 Tests

- `tests/` loads frozen legacy fixtures (see `TestFileFactory`,
  `TestChartView`). **Do not regenerate fixtures during the rename** — they
  are regression oracles for the legacy v1 binary format. The rename must not
  break reading them.
- `tests/CMakeLists.txt` uses an explicit `crochet_app_cpp` list (not a
  `file(GLOB …)`). If any source file is renamed as part of the rebrand,
  update this list.

### 3.6 What NOT to rename

- Upstream author names, pre-fork copyright years, pre-fork commit messages,
  pre-fork tag names.
- The `Stitch Works Software` string inside existing GPL headers
  (add, don't overwrite).
- Third-party scripts' embedded branding (`gprof2dot.py`,
  `asan_symbolize.py`, CMake helpers).

---

## 4. Suggested order of operations

Each numbered item is intended as one commit (or a small, reviewable series):

1. **Decide** new name and package identifier (`com.<newdomain>.<newname>`).
2. **Compliance pre-work** (no rename yet):
   - Add `AUTHORS`, `NOTICE`.
   - Add `LICENSES/CC-BY-SA-4.0.txt` and an `ARTWORK.md`.
   - Add a "Fork of …" paragraph to `README.md`.
   - Fix the `src/scene.h:2` "C opyright" typo.
3. **Maintainer copyright sweep:** add the new maintainer's copyright line
   and a "modified from CrochetCharts" notice to every file you have already
   substantively changed. Optionally add `SPDX-License-Identifier:
   GPL-3.0-or-later` while you are there.
4. **Build/packaging rename:** CMake project name, CPack identifiers, bundle
   id, `.desktop`, NSIS, debian scripts, Taskfile, resource filenames.
   Verify `task build` and `task build:release` still work.
5. **App-identity artwork swap:** splash, app icon, `.iconset` bundles,
   Windows `.rc` icon. Verify the binary launches and shows the new icon.
6. **User-visible strings:** window title, About dialog, splash text, docs
   (`README.md`, `AGENTS.md`, `docs/architecture/**`,
   `docs/index.docbook.in`).
7. **File-format compatibility verification:** open a pre-rename `.pattern`
   file end-to-end; ensure `TestFileFactory` / `TestChartView` still pass
   (`task test`).
8. **Repo rename** on GitHub, update local remote, update README/badge URLs.
9. **Tag a release** so `git describe` produces a clean version under the
   new name.

---

## 5. Minimum-effort path ("just don't get sued")

If you want the smallest set of actions that still satisfies both licenses:

- Keep `LICENSE` (GPLv3).
- Keep every existing copyright header. Do not edit them.
- Add your own copyright line to files you actually modify.
- Add `AUTHORS`, `NOTICE`, and `LICENSES/CC-BY-SA-4.0.txt`.
- Add a "Fork of CrochetCharts, originally by …" line to the top of
  `README.md`.
- Pick a clearly distinct new name.
- Replace the app icon and splash.

Everything else in this document is polish, better ergonomics, or
back-compat for existing users — not a legal requirement.

---

## 6. References

- GNU GPL v3 — `LICENSE` (this repo); canonical:
  <https://www.gnu.org/licenses/gpl-3.0.html>
- CC BY-SA 4.0 legal code:
  <https://creativecommons.org/licenses/by-sa/4.0/legalcode>
- SPDX license list: <https://spdx.org/licenses/>
- Upstream (archived):
  <https://github.com/StitchworksSoftware/CrochetCharts>
