---
name: arc42-sync
description: Keep the CrochetCharts arc42 architecture documentation under docs/architecture/ in sync with the code. Use when editing code or build config that may invalidate architectural statements — adding/removing modules or classes under src/, changing the project file format, touching .devcontainer/ or CPack/resources/installers.cmake, adding/removing singletons, adding a new Scene mode, adding/removing external dependencies, adding/removing QUndoCommand subclasses, making load-bearing architecture decisions, or introducing/paying down technical debt. Also use when the user explicitly asks to update the architecture doc.
---

# arc42 documentation sync

The `docs/architecture/` tree is a living arc42 document split into 12 sections plus a README/TOC. It must stay in sync with the code — stale architecture docs are worse than none.

**Core rule:** if a code change would make a statement in `docs/architecture/` wrong, update the doc **in the same commit** as the code change. Do not defer.

## When this skill applies

Use the trigger table below. If a commit touches any of the left-column things, edit the listed sections:

| Change | Section(s) to update |
|---|---|
| Add/remove a top-level module or class in `src/` | `05-building-blocks.md` (block table + mermaid diagrams), `12-glossary.md` if it introduces a new term |
| Change the file format (new `file_vN.{cpp,h}`, new persisted field, header change) | `05-building-blocks.md` § 5.4, `06-runtime.md` § 6.2/6.4, `08-crosscutting.md` § 8.3, `09-decisions.md` (new ADR if the versioning strategy itself changes) |
| Change `.devcontainer/` (base image, user, installed tools, X11 setup) | `07-deployment.md` § 7.1, and add/update an ADR in `09-decisions.md` |
| Change CPack / packaging / `resources/installers.cmake` / platform assets | `07-deployment.md` § 7.2 |
| Add / remove a singleton | `08-crosscutting.md` § 8.2 |
| Add a new `Scene` interaction mode (enum value + event branches) | `08-crosscutting.md` § 8.5 (mode checklist), `11-risks-and-debt.md` D-1 (god class size) |
| Add / remove an external dependency (Hunspell, a new lib) | `02-constraints.md` § 2.1, `03-context.md` § 3.3 |
| Add / remove a `QUndoCommand` subclass | `08-crosscutting.md` § 8.1 (keep the "≈ 18" count honest), `06-runtime.md` § 6.3 if a new flow is interesting |
| Make a load-bearing architecture decision (language level, threading, Qt version, storage, packaging strategy) | `09-decisions.md` — add the next-numbered ADR |
| Eliminate or introduce technical debt | `11-risks-and-debt.md` § 11.2 |
| Change the quality bar (new perf target, coverage goal, quality scenario) | `10-quality.md` § 10.2 |
| Rename a file / class / directory referenced with a line number | `grep -r "old_path" docs/architecture/` and update citations |

## Workflow

1. **Identify the change class.** Map your code change to a row in the trigger table. If multiple rows apply, open each affected section.

2. **Read the current section.** The existing text shows the prose style and level of detail. Match it.

3. **Edit minimally.** Change the smallest coherent unit — a table row, a diagram node, a single bullet. Full-section rewrites invite drift and noisy diffs.

4. **Keep the mermaid diagrams honest.** `05-building-blocks.md` and `07-deployment.md` hold the diagrams most likely to drift. Add/rename nodes; do not let the diagram and the prose disagree.

5. **File:line citations.** New citations should be accurate. Existing citations do not need to be re-verified on every commit — fix them opportunistically when you happen to touch nearby code.

6. **If you made a load-bearing decision, add an ADR.** Use the next ADR number in `09-decisions.md`. Format:
   - `## ADR-NN · Title`
   - `- **Status:** Accepted, YYYY-MM.`
   - `- **Context:** …`
   - `- **Decision:** …`
   - `- **Rationale:** …`
   - `- **Consequences:** …`
   - Add a "Revisit if" clause when the decision is reversible.

7. **Cross-reference.** If you add a new section anchor, link it from sibling sections where it's relevant (e.g. a new ADR should be referenced from § 4 Solution Strategy if it is load-bearing).

8. **Commit together.** The architecture doc edit and the code change land in the same commit. Use a commit message that mentions both:
   ```
   feat(scene): add RulerMode for measurement

   Adds a new Scene::mMode value and the three mouse-event branches.
   Updates arc42 § 8.5 with the new mode and grows D-1 in § 11.2
   (god class now 3700 lines).
   ```

## Style rules

- **Audience:** skilled engineers. Skip introductory material.
- **Direct.** No "this section will explain…" preambles.
- **Tables over prose** when listing ≥ 3 items with the same shape.
- **Mermaid** for all diagrams.
- **No emojis** unless the user explicitly requests them.
- **German-free**: project and doc are English even if the dev host is German-localised.

## Do NOT

- **Do not duplicate content** between `AGENTS.md` and `docs/architecture/`. `AGENTS.md` is the operational index (where things live, commands, anti-patterns). The arc42 doc is the architectural narrative. Cross-reference, don't copy.
- **Do not put end-user help** in the arc42 doc. That belongs in `docs/index.docbook.in`.
- **Do not rewrite old ADRs when reversing them.** Add a new ADR that supersedes the old, and mark the old one `Status: Superseded by ADR-NN`.
- **Do not invent sections.** The 12-section arc42 shape is fixed. Sub-sections (5.3, 8.5, …) may grow as needed.
- **Do not defer "I'll update the doc in a follow-up".** Follow-ups rot.

## Quick reference — file map

| File | Section |
|---|---|
| `docs/architecture/README.md` | Index + TOC + how-to |
| `docs/architecture/01-introduction-and-goals.md` | Purpose, features, quality goals, stakeholders |
| `docs/architecture/02-constraints.md` | Technical + organisational constraints, conventions |
| `docs/architecture/03-context.md` | Business + technical context diagrams, external deps |
| `docs/architecture/04-solution-strategy.md` | S1–S9 load-bearing strategy choices |
| `docs/architecture/05-building-blocks.md` | Levels 1–3 static structure, ownership cheatsheet |
| `docs/architecture/06-runtime.md` | Sequence diagrams: startup, open, place, save, export, crash |
| `docs/architecture/07-deployment.md` | Dev container + end-user packaging per platform |
| `docs/architecture/08-crosscutting.md` | Undo, singletons, friend-serializers, resources, modes, etc. |
| `docs/architecture/09-decisions.md` | ADR log |
| `docs/architecture/10-quality.md` | Quality tree + scenarios QS-1…N |
| `docs/architecture/11-risks-and-debt.md` | R-1…N risks, D-1…N debts |
| `docs/architecture/12-glossary.md` | Domain + technical terms, paths |

## Verification

Before finalising the commit:

- `grep -rn "TODO" docs/architecture/` — no stray TODOs introduced.
- `grep -l "ADR-NN" docs/architecture/` — no placeholder ADR numbers left.
- If you added a section anchor, verify internal links still resolve (relative `.md` links to `file.md#anchor`).
- If a diagram mentions a file path, make sure it still exists.
