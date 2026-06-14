# 9. Architecture Decisions

Lightweight ADR log — one entry per load-bearing choice. Focused on what is *current*, not historical waffle.

Format: `ID · title · status · context · decision · consequences`.

---

## ADR-01 · Stay on Qt 4.8

- **Status:** Accepted, likely permanent.
- **Context:** Qt 4 is EOL upstream. Qt 5/6 exist and are actively supported. Every new OS release makes Qt 4 harder to install.
- **Decision:** Do not port. `find_package(Qt4 REQUIRED)` stays in `CMakeLists.txt`.
- **Rationale:** The port effort is large (SIGNAL/SLOT syntax, `QString` API deltas, `qInstallMsgHandler`→`qInstallMessageHandler`, `QtGui`/`QtWidgets` split, `QGraphicsView` subtleties, Qt4-specific DocBook integration). The application is feature-complete. No user-facing benefit justifies the cost.
- **Consequences:**
  - Linux distribution is effectively pinned to Ubuntu 18.04 / EOL distros (and the dev container does the same).
  - Cannot use modern C++ features that Qt 5/6 idioms assume.
  - Security: no upstream fixes — mitigated by the app having no network attack surface by default (updater is opt-in).
- **Revisit if:** a motivated contributor commits to a port *with* test coverage that exceeds today's 5 %, or Qt 4 becomes unbuildable on any supported target.

## ADR-02 · Upgrade git via `ppa:git-core/ppa` inside the container

- **Status:** Accepted, 2026-04.
- **Context:** Ubuntu 18.04 ships git 2.17 (2018). Modern tooling (GitHub Desktop features, signing flows, sparse-checkout, maintenance) assumes ≥ 2.30.
- **Decision:** Enable `software-properties-common` + `add-apt-repository -y ppa:git-core/ppa` in the Dockerfile. Pulls git 2.50+.
- **Rationale:** Zero-cost upgrade, distro-maintained, signed. Alternative (build from source) costs minutes per image rebuild.
- **Consequences:** A 18.04 base with a PPA overlay. If the PPA ever drops Bionic, the Dockerfile breaks and will need the source-build fallback.

## ADR-03 · Non-root `vscode` user (UID 1000) inside the container

- **Status:** Accepted, 2026-04.
- **Context:** Earlier the container ran as root. Files created via the bind mount ended up `root:root` on the host, which the IDE could not delete without `sudo`.
- **Decision:** Add a `vscode` user with UID/GID 1000 and passwordless sudo. Set `remoteUser` / `containerUser` in `devcontainer.json`.
- **Rationale:** Matches the typical host UID (1000), so bind-mount writes are host-owned. Passwordless sudo lets the IDE run package installs or chown fixes without password prompts.
- **Consequences:**
  - Host users with a different UID need to rebuild with `--build-arg USER_UID=<their-uid>`. Currently not parameterised in `devcontainer.json`.
  - Tests that expect root are none at present.

## ADR-04 · `QGraphicsScene` as canvas, `Cell` = `QGraphicsSvgItem`

- **Status:** Accepted.
- **Context:** A crochet chart is a lot of small, interactive, z-ordered, freely transformed SVG items.
- **Decision:** Use Qt's scene graph. Each stitch is a `QGraphicsSvgItem` subclass that knows its `Stitch*`, colour, and transform.
- **Consequences:**
  - Free hit-testing, z-order, transforms, selection.
  - Scene owns items transitively; ownership rules (§ 5.5) must be respected.
  - Data model and presentation are the same object — no clean MVC path.

## ADR-05 · Versioned file format with frozen past versions

- **Status:** Accepted.
- **Context:** Project files from 2010+ must keep opening.
- **Decision:** `FileFactory` reads a header, constructs `File_vN`. New versions are new classes (`file_v3.{cpp,h}`, etc.). v1 and v2 are frozen.
- **Consequences:**
  - Schema can evolve without risking existing files.
  - Code duplication between versions is accepted — better than back-patching a frozen version.
  - `friend class` grant on data classes is the price paid for compact serializer code (ADR-06).

## ADR-06 · `friend class` for serializers

- **Status:** Accepted, debt acknowledged.
- **Context:** Data classes (`Cell`, `Scene`, `CrochetTab`, `Indicator`) have many private fields. Making each serialisable via public accessors inflates surface area.
- **Decision:** Declare `friend class File_v1/v2/FileFactory/SaveFile` on those data classes. Serializers read/write private fields directly.
- **Consequences:**
  - Tight but explicit coupling between serializer and data layout. Adding a field is one line on the class and one line in the serializer.
  - Refactoring a data class can break serialization silently. Tests in `tests/testfilefactory.cpp` are the safety net — expand them when adding fields.

## ADR-07 · Qt resource system for built-in stitches

- **Status:** Accepted.
- **Context:** Distributing a binary that expects a sibling `stitches/` directory is fragile on Windows/macOS.
- **Decision:** Compile stitches into the binary via `stitches/stitches.qrc`. User-added stitches live in a user data directory as `.set` files.
- **Consequences:**
  - One-file deployment; paths are `:/stitches/…`.
  - Updating a built-in SVG requires a rebuild.
  - Disk footprint of binary ≈ 5–10 MB larger than a sibling-assets layout.

## ADR-08 · No threading

- **Status:** Accepted.
- **Context:** Qt supports `QThread`; the codebase does not use it. Export and file I/O are synchronous.
- **Decision:** Stay single-threaded on the GUI thread. Long operations show a busy cursor.
- **Consequences:**
  - No race conditions, no thread-safety burden on singletons or `Scene`.
  - Very large exports or saves can stutter. Acceptable given target chart size (≤ a few thousand cells).
  - A future threading effort has to first retrofit thread-safety onto singletons and break `Scene`'s god-class state.

## ADR-09 · Single-binary packaging per platform (CPack)

- **Status:** Accepted.
- **Context:** Three platforms, three packaging ecosystems (NSIS, Bundle, DEB/RPM).
- **Decision:** Drive all of them from one `resources/installers.cmake` using CPack generators.
- **Consequences:**
  - All platform metadata (icons, plists, `.desktop`, MIME XML, NSIS script, signing cert lists) is version-controlled in one place.
  - Platform-specific quirks leak into `installers.cmake` — it has become long and hard to navigate.
  - Per-platform external assets (certs, NSIS install) are still required out-of-tree.

## ADR-10 · DocBook for the end-user manual

- **Status:** Accepted, lightly maintained.
- **Context:** `docs/index.docbook.in` produces HTML/PDF/Pages.
- **Decision:** Keep DocBook. Opt-in via `-DDOCS=ON`; requires `xsltproc` + Apache FOP + `docbook-xsl-ns` on the build host. `DocbookGen.cmake` invokes the tools via `execute_process()` during cmake configure, so a single `cmake -DDOCS=ON` produces the manual — no `make` step.
- **Consequences:**
  - End-user manual stays versioned with the code.
  - Toolchain is arcane; new maintainers tend not to update the manual. Revisit if manual drifts badly.
  - The dev container bundles `xsltproc`, `fop`, `docbook-xsl`, `docbook-xsl-ns` so `task docs` works out of the box (2026-04 onward). Adds ~200 MB to the image (JRE for FOP) but removes a friction point for contributors.
