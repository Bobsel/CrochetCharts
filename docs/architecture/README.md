# CrochetCharts Architecture (arc42)

This directory holds the architecture documentation for CrochetCharts. It follows the [arc42](https://arc42.org/) template, pragmatically condensed for a ~23 kLOC single-binary Qt4 desktop application.

**Audience:** engineers modifying the codebase. Familiarity with Qt4, CMake, and desktop application patterns is assumed. Introductory material is skipped.

**Scope:** this document describes the *as-is* architecture of the CrochetCharts app, including known debt. It is not a roadmap.

## Table of Contents

| # | Section | File | Focus |
|---|---------|------|-------|
| 1 | Introduction and Goals | [01-introduction-and-goals.md](01-introduction-and-goals.md) | What the product does, quality goals, stakeholders |
| 2 | Architecture Constraints | [02-constraints.md](02-constraints.md) | Hard boundaries — Qt4, C++11, GPL, desktop-only |
| 3 | System Context and Scope | [03-context.md](03-context.md) | External actors, file formats, OS interfaces |
| 4 | Solution Strategy | [04-solution-strategy.md](04-solution-strategy.md) | Load-bearing design choices in one page |
| 5 | Building Block View | [05-building-blocks.md](05-building-blocks.md) | Static structure, level 1–3, top-down |
| 6 | Runtime View | [06-runtime.md](06-runtime.md) | Key scenarios as sequence diagrams |
| 7 | Deployment View | [07-deployment.md](07-deployment.md) | Dev container, build targets, packaging |
| 8 | Crosscutting Concepts | [08-crosscutting.md](08-crosscutting.md) | Patterns applied across modules |
| 9 | Architecture Decisions | [09-decisions.md](09-decisions.md) | Decision log with rationale |
| 10 | Quality Requirements | [10-quality.md](10-quality.md) | Quality tree and scenarios |
| 11 | Risks and Technical Debt | [11-risks-and-debt.md](11-risks-and-debt.md) | Known debt with remediation cost |
| 12 | Glossary | [12-glossary.md](12-glossary.md) | Domain and technical terms |

## How this doc stays correct

This document MUST be kept in sync with code. See `AGENTS.md` → "Architecture doc sync" for the rules. In short: changes to public structure, file formats, build, deployment, or known tech debt require a matching edit here in the same commit.

## Conventions used in this doc

- **Mermaid** for all diagrams. GitHub and most modern viewers render them inline.
- **File paths with line numbers** (`src/scene.cpp:540`) point to concrete evidence in the code. These may drift; they are correct as of the commit that last touched this file.
- **Sidebars** (> blockquotes) flag non-obvious rationale or gotchas.

## Related documents

- `AGENTS.md` — operational map (where things live, commands, anti-patterns). Agents-first, narrow.
- `src/AGENTS.md`, `tests/AGENTS.md`, `stitches/AGENTS.md` — per-area guides.
- `README.md` — end-user quick start, license notes.
- `docs/index.docbook.in` — DocBook user manual source (end-user facing, unrelated to this doc).
