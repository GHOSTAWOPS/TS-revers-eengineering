# STEP-only + RebarSmart Evidence + Detail Compatibility Roadmap

This is the public-safe roadmap for the 2026-06-11 route pivot. It intentionally avoids committing company-internal RebarSmart binaries, resource files, customer models, license files, IDA databases, or detailed private reverse-engineering listings.

## Summary

The project should continue from the existing Qt6 + OCCT app rather than restart.

New route:

```text
STEP-only independent platform
  + RebarSmart evidence for steel generation logic
  + old Tushi / VisualTS evidence for Detail package compatibility
  + Qt6 + OCCT as the only long-term runtime foundation
```

Not the current route:

```text
Full VisualTS UI / operation flow 1:1 as P0
Direct RebarSmart runtime integration
Direct VisualTS runtime integration
ACIS / HOOPS / 3DE / CAA dependency
Full .sfl / SAT / SAB / DWG / DXF compatibility as P0
```

## Architecture

```text
STEP/STP
  -> OCCT STEP Importer
  -> TopologyBinding / Stable FaceId / EdgeId
  -> Qt6 + OCCT AIS selection and display
  -> RebarSmart-style generator layer
  -> RebarModel
  -> DrawingModel
  -> DetailPackageExporter / PDF / SVG / Excel / XML / STEP
```

## Runtime Boundary

Allowed:

- C++.
- Qt6.
- OCCT.
- Self-developed RebarModel.
- Self-developed DrawingModel.
- Self-developed DetailPackageReader / Writer / Exporter.
- Open-source helper libraries after license review.

Forbidden:

- RebarSmart runtime binaries.
- VisualTS runtime binaries.
- 3DE / CAA / CATIA runtime.
- ACIS / Spatial.
- HOOPS.
- Codejock.
- USB dongle or license service as a new-system runtime requirement.

## Existing Assets To Keep

- `app` Qt6 / OCCT project.
- STEP import and AIS viewer.
- Selection reference work.
- `domain/rebar` model foundation.
- `.tsrebar` project package direction.
- Existing Detail writer evidence and tests.
- Dependency gate, readiness gate, and documentation workflow.

`LegacyGeometryAdapter` remains useful as historical VisualTS geometry support and compatibility evidence, but new RebarSmart-style generators should be designed around a clean `IGeometryEngine` boundary.

## Evidence Strategy

Steel generation:

1. RebarSmart evidence: parameters, defaults, spacing, distribution, guide curves/surfaces, hooks, generator categories.
2. Old Tushi exported STP / Detail / schedule outputs as result witnesses.
3. VisualTS / IDA only as supplementary evidence when needed.
4. Parent directory code only as engineering reference.

Drawing / Detail:

1. Real old Tushi `Detail.xml + DetailNN.stl` packages.
2. Old AutoCAD plugin import behavior.
3. VisualTS / plugin static evidence.
4. Current DetailWriter and DrawingModel tests.

## Phase Plan

### M0: Route Governance

Deliverables:

- ADR in `docs/adr`.
- Roadmap in `docs/roadmap`.
- Updated `README.md`, `AGENTS.md`, `00`, `06`, `07`, `11`, `34`, `46`, `99`, and `todo.csv`.
- Old VisualTS 1:1 route marked historical / downgraded.
- `TODO-090` set as the next task.

### M1: Detail Package POC

Deliverables:

- `DetailPackage` data model.
- `DetailPackageReader`.
- `DetailPackageWriter` round-trip.
- Minimal generated Detail package.
- Manual old AutoCAD plugin validation checklist.

Acceptance:

- Old package can be read.
- Old package can be round-tripped with conservative field preservation.
- Minimal one-view / one-line package can be generated.
- CAD plugin validation is recorded as pass, fail, or blocked with evidence.

### M2: RebarSmart-Style Pure Algorithm Layer

Deliverables:

- `GenerateRebarData`.
- INI/default loader with explicit non-UTF8 handling.
- Space list parser.
- Distribution calculators.
- Guide curve zone calculator.
- Unit tests independent of OCCT.

Acceptance:

- Pure algorithm tests pass without 3DE / CAA / RebarSmart runtime.
- Low-confidence fields are marked unresolved rather than asserted as fact.

### M3: Geometry Interface And OCCT Engine

Deliverables:

- `IGeometryEngine`.
- Mock geometry engine for pure algorithm tests.
- OCCT implementation for curve length, point by length, tangent, line/arc/wire construction, projection, offset, section, and sweep as needed.

Acceptance:

- Generator layer depends on `IGeometryEngine`, not raw OCCT.
- `domain/rebar` still has no OCCT / AIS dependency.

### M4: Qt / OCCT Operation Loop

Deliverables:

- Open STEP.
- Select face / edge / point.
- Edit generator parameters.
- Generate steel group.
- Display in AIS.
- Save and reopen `.tsrebar`.
- Export basic schedule.

### M5: Detail Export From RebarModel

Deliverables:

- Rebar group to Detail steel group mapping.
- Segment to Detail geometry mapping.
- Schedule to steel/material table mapping.
- DrawingModel to section/hidden/center/hatch line mapping.
- Detail package export usable by old CAD plugin.

## Initial Todo Queue

```text
TODO-090  DetailPackage data model P0
TODO-091  DetailPackageReader P0
TODO-092  DetailPackageWriter round-trip P0
TODO-093  Minimal Detail package generation P0
TODO-094  GenerateRebarData P0
TODO-095  RebarSmartIniLoader encoding and units P0
TODO-096  SpaceListParser P0
TODO-097  Distribution P0
TODO-098  GuideCurveZoneCalculator P0
TODO-099  IGeometryEngine P0
```

## Validation

Per node:

- Relevant unit/integration tests.
- Default CTest when code changes.
- Readiness gate.
- `domain/rebar` OCCT/AIS leak scan when relevant.
- xhigh read-only review for code/test/build changes.
- Implementation record, build report, traceability matrix, gap log, `46`, and `todo.csv` updates.
- Commit, annotated tag, push.

## Risks

- Detail package field tolerance is unknown until old CAD plugin validation.
- RebarSmart evidence must be rewritten into self-owned code.
- INI/default files may use non-UTF8 encodings and mixed units.
- OCCT geometry may differ from 3DE / ACIS and needs golden cases.
- Public repository must not receive proprietary binaries, private samples, or sensitive reverse-engineering artifacts.
