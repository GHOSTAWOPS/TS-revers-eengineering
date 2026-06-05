# Phase 1 Readiness Run 001

## Summary

- Date: 2026-06-05
- Operator: Codex
- Result: M1-Formal
- Decision: M1-Formal is allowed for Qt6 + OCCT engineering initialization.

## Scope

- Tabs: 开始 / 显示 / 钢筋 / 查询 / 工程图
- Excluded: 堆石坝 / 老 SFL 直接导入 / 直接 DWG DXF 写出
- Parent source policy: reference-only

## Linked Requirements

- REQ-PROJ-001
- REQ-TECH-001
- REQ-TECH-002

## Linked Evidence

- E-DEV-005
- E-DEV-006
- E-DEV-007
- E-DEV-008
- E-DEV-009
- E-DEV-010
- E-DEV-011
- E-DEV-012
- E-DEV-013
- E-DEV-014
- E-DEV-015
- E-DEV-016
- E-DEV-017
- E-DEV-018
- E-DEV-019
- E-DEV-020
- E-DEV-021
- E-DEV-022
- E-TECH-002
- E-TECH-003
- E-TECH-004
- E-TECH-005

## Linked GAP

- GAP-DEV-002
- GAP-DEV-001 and GAP-DEV-007 are closed for the M1 entry gate by E-DEV-022.

## Artifact Inventory

- Fixture A: `fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/`
- Fixture B: `fixtures/tsrebar/fixture_b_single_group/project.tsrebar/`
- Fixture C: `fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/`
- Negative fixtures: `fixtures/tsrebar/negative/`
- Validator reports: `docs/phase1/validator_reports/`
- Save/Open reports: `docs/phase1/save_open_reports/` (standalone transaction gate only)
- Qt6 app Save/Open templates, simulation reports and runtime reports: `docs/phase1/app_save_open_reports/`
- STEP diff reports: `docs/phase1/step_selection/` (pass for current seed samples; GAP-DEV-002 remains P1 residual for new engineering STEP samples)
- Readiness gate reports: `docs/phase1/readiness_gate_reports/` (current decision: M1-Formal-Ready)
- Dependency artifacts: `DEPENDENCIES.md`, `THIRD_PARTY_NOTICES.md`, `licenses/`, `sbom/project-sbom.spdx.json`
- Dependency probe report: `docs/phase1/dependency_probe_run_001.md` (local vcpkg candidate found)
- Dependency gate report: `docs/phase1/dependency_gate_run_001.md` (current result: pass)
- Qt6 application Save/Open report: present, current decision `qt6-app-pass`
- Binding repair run report: present, current decision `qt6-binding-repair-pass`
- Detail writer L0/L1 reports: present, current decision `l0-l1-pass`

## Dependency Gate

- DEPENDENCIES.md: present, frozen to local vcpkg candidate
- THIRD_PARTY_NOTICES.md: present, frozen to local vcpkg candidate
- licenses: present, source availability and copyright paths recorded
- sbom: present, Qt/OCCT versionInfo and downloadLocation filled
- dependency probe report: present
- dependency gate report: present
- dependency gate command: `tools/dependency_gate/check_dependencies.py`
- current result: pass
- current warning: CMake project not created yet, so CMake module scan is skipped until real Qt6 + OCCT project initialization

## Fixture Gate

- Fixture A: present as actual skeleton; validator report exists
- Fixture B: present as actual skeleton; validator report exists
- Fixture C: present as actual skeleton; validator report exists
- Negative fixtures: present as actual negative skeletons; validator reports exist
- golden assertion coverage: limited to current seed fixtures
- result: pass for M1 entry; broaden when new engineering samples arrive

## Validator Gate

- PackageValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- ProjectReferenceValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- LegacyObjectValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- GeometryRefValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- LegacyGeometryBindingValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- CoreObjectBindingValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- DetailMappingValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- EvidenceValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- TransactionValidator: implemented in `tools/tsrebar_validator/validate_tsrebar.py`; first reports exist
- result: pass for current M1 entry gate; broaden with real project samples during development

## Save/Open Gate

- Save success: standalone transaction gate run exists; app state-machine simulation exists; Qt6 C++ runtime gate run exists
- Save validator failure: standalone transaction gate run exists; app state-machine simulation exists; Qt6 C++ runtime gate run exists
- Save interrupted: not run
- Open success: standalone transaction gate run exists; app state-machine simulation exists; Qt6 C++ runtime gate run exists
- Open STEP missing: standalone blocked-open case exists; app state-machine simulation exists; Qt6 C++ runtime gate run exists
- Open topology mismatch: not run
- Application-level simulation: `tools/app_save_open_gate/app_gate.py`
- Application-level simulation report: `docs/phase1/app_save_open_reports/app_save_open_run_001.json`
- Qt6 runtime gate: `tools/qt6_runtime_gate/run_qt6_runtime_gate.py`
- current runtime decision: `qt6-app-pass`
- result: pass for M1 entry; later product code must carry the same state machine

## Binding Repair Gate

- Preview runtime report: `docs/phase1/app_save_open_reports/binding_repair_preview_001.md`
- Apply runtime report: `docs/phase1/app_save_open_reports/binding_repair_run_001.json`
- Cancel runtime report: `docs/phase1/app_save_open_reports/binding_repair_cancel_001.json`
- current runtime decision: `qt6-binding-repair-pass`
- result: pass for M1 entry; later product code must keep preview/apply/cancel guardrails

## STEP Selection Gate

- sample: C:/Users/ghost/Desktop/reverse_engineering/test.stp
- run_r1: present
- run_r2: present
- run_r3: present
- run_r4: present
- run_r5: present with copied STEP path
- diff: `docs/phase1/step_selection/test_stp/step_selection_diff.md`
- face match rate: 1.000000
- edge match rate: 1.000000
- vertex match rate: 1.000000
- unresolvedGeometry cases: none for current seed samples
- additional passing samples: `docs/phase1/step_selection/cc001_bspline_edge/`, `cc002_bezier_edge/`, `cc003_bspline_length/`, `cc004_hostref_source_curve/`, `cc005_sampling_policy/`
- decision: pass for current seed samples; GAP-DEV-002 moved to P1 residual

## Detail Writer Gate

- L0 XML: passed
- L1 ID same-source: passed
- rollback: passed; old package hash preserved on failure probe
- AutoCAD L2: not run
- result: L0/L1 pass; AutoCAD L2 remains a drawing-stage residual, not a current M1 readiness blocker

## Findings

| ID | Severity | Gate | Evidence | Decision |
|---|---|---|---|---|
| R001 | warning | Fixture | Fixture A/B/C, negative fixtures and validator reports exist; golden coverage is still limited to seeded fixtures | Pass for M1 entry; expand during development |
| R002 | info | Validator / SaveOpen | E-DEV-015/E-DEV-016/E-DEV-020/E-DEV-022 exist; standalone validator, standalone Save/Open, app state-machine simulation and Qt6 runtime reports exist | M1-Formal allowed |
| R003 | warning | STEP | E-DEV-009/E-DEV-017 exist; current seed STEP samples pass with `step-shape-fingerprint/v1`; future real project STEP samples must keep this gate | GAP-DEV-002 residual P1 |
| R004 | info | Dependency | E-TECH-002/E-TECH-003/E-TECH-004/E-TECH-005 exist; local vcpkg dependency candidate is frozen and dependency gate reports `pass` | CMake allowlist scan continues after project creation |
| R005 | info | BindingRepair | E-DEV-018/E-DEV-020/E-DEV-022 exist; binding repair preview/apply/cancel runtime reports exist and pass | M1-Formal allowed |
| R006 | info | ReadinessGate | E-DEV-019/E-DEV-021/E-DEV-022/E-TECH-005 exist; readiness gate run 001 reports 78/78 pass and 0 errors | M1-Formal allowed |
| R007 | info | DetailWriter | E-DEV-021 exists; Detail writer L0/L1 offline gate passes and preserves old package on failure probe | AutoCAD L2 not claimed |

## Final Decision

- M1-Prep allowed: yes
- M1-Formal allowed: yes
- Remaining M1 blockers: none
- Residual after entry: GAP-DEV-002 remains for new engineering STEP samples;旧图石业务功能缺口继续按 IDA / 旧图石运行 / SFL / Detail 证据闭合。
