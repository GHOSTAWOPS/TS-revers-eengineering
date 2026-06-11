# ADR-20260611: STEP-only + RebarSmart Evidence + Tushi Detail Compatibility

Status: Accepted

Date: 2026-06-11

Public-safety note: this repository version is a route-level summary. It must not include RebarSmart binaries, proprietary resource files, customer models, license files, IDA databases, or detailed private reverse-engineering listings.

## Context

The previous route attempted to reproduce old VisualTS / Tushi behavior as fully as possible by using Qt6 and OCCT to replace MFC, HOOPS, and ACIS. That produced useful assets:

- Qt6 app skeleton.
- OCCT STEP import and AIS viewer.
- Selection references.
- Rebar domain model.
- `.tsrebar` project package direction.
- Initial Detail writer work.
- Tests, gates, dependency notices, and documentation discipline.

After reviewing the real effort, the old route is too slow for the current objective. A company-internal RebarSmart 3DE/CAA based system appears to expose clearer steel generation logic than VisualTS, while old Tushi remains valuable for Detail package and CAD plugin compatibility evidence.

## Decision

The project route is changed to:

```text
STEP/STP input
  -> Qt6 + OCCT independent platform
  -> RebarSmart-style steel generation reimplemented from evidence
  -> RebarModel / DrawingModel / ScheduleModel
  -> DetailPackageExporter for old Tushi CAD plugin compatibility
```

Runtime dependencies are limited to:

- C++.
- Qt6.
- OCCT.
- Self-developed models and exporters.
- Necessary open-source XML / JSON / CSV / XLSX helper libraries.

Runtime must not depend on:

- RebarSmart binaries or 3DE / CAA runtime.
- VisualTS binaries.
- ACIS / Spatial.
- HOOPS.
- Codejock.
- USB dongle, HASP, network license, or old license services.

## Evidence Roles

RebarSmart evidence is used for:

- Steel generation parameters.
- Default values.
- Spacing and distribution logic.
- Guide curve / guide surface concepts.
- Hook, anchor, and shape rules.
- Generator classification.

VisualTS / old Tushi evidence is used for:

- Detail XML package structure.
- Drawing and schedule field mapping.
- Old AutoCAD plugin compatibility.
- Historical comparison and fallback evidence.

The old AutoCAD plugin is only a compatibility validation target. It is not a core dependency of the new system.

## Data Format Decision

`.tsrebar` remains the internal project format.

Detail package is an exporter format only:

```text
RebarModel + DrawingModel + ScheduleModel
  -> DetailPackageExporter
  -> Detail.xml + DetailNN.stl
```

Important: in this compatibility chain, `DetailNN.stl` is XML text, not standard STL mesh data.

## Consequences

Positive:

- Keeps existing Qt6 / OCCT app assets.
- Avoids ACIS / HOOPS / VisualTS runtime dependency.
- Avoids direct 3DE / CAA runtime dependency.
- Uses the clearer RebarSmart evidence path for steel generation.
- Prioritizes Detail package compatibility before direct DWG/DXF writing.

Negative:

- RebarSmart evidence must still be converted into self-owned code.
- Detail package Reader / Writer / Exporter must be rebuilt cleanly.
- OCCT geometric results may differ from 3DE / ACIS and require golden validation.
- Old VisualTS UI 1:1 reproduction is no longer P0/P1.

## Superseded Goals

The following are downgraded to P2 or later special topics:

- Full old VisualTS UI and operation flow 1:1 reproduction.
- Full `.sfl` compatibility.
- Direct SAT / SAB / DWG / DXF compatibility.
- Calling RebarSmart, VisualTS, 3DE, ACIS, HOOPS, or Codejock at runtime.

## Near-Term Roadmap

1. M0: route governance and documentation pivot.
2. M1: DetailPackage model, Reader, Writer round-trip, minimal package.
3. M2: RebarSmart-style pure algorithm layer.
4. M3: geometry interface and OCCT implementation for generators.
5. M4: Qt / OCCT operation loop.
6. M5: RebarModel to DrawingModel to Detail package export.

## Guardrails

- Do not commit RebarSmart binaries/resources or private reverse-engineering listings.
- Do not let `domain/rebar` depend on OCCT / AIS.
- Do not use Detail XML fields as the internal business model.
- Do not treat old VisualTS UI parity as a current blocker.
- Record uncertain evidence as gap, not fact.
