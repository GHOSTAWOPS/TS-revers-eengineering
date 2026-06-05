# Validator Reports Run 001

## Summary

- Date: 2026-06-05
- Runner: `tools/tsrebar_validator/validate_tsrebar.py`
- Test command: `python tools/tsrebar_validator/test_validate_tsrebar.py`
- Result: validator implementation and reports exist; M1-Formal is still not allowed.
- Evidence: `E-DEV-015`

## Scope

This run validates the current `*.tsrebar` fixture contract:

- package required files
- project references
- legacy raw object presence
- geometryRef constraints
- legacyObject to geometry binding
- Detail rsd same-source mapping
- evidence references and pending fields
- save transaction state

It does not validate:

- old VisualTS runtime behavior
- OCCT STEP import stability
- Save/Open atomic rollback in the Qt6 application
- Detail writer XML output against AutoCAD

## Positive Fixtures

| Report | Decision | Errors | Warnings | Meaning |
|---|---:|---:|---:|---|
| `fixture_a_empty_step_validation_001.json` | pass | 0 | 0 | Empty package contract is valid. |
| `fixture_b_single_group_validation_001.json` | warning-only | 0 | 13 | One-group fixture is structurally valid; pending legacy fields remain open. |
| `fixture_c_pending_legacy_validation_001.json` | warning-only | 0 | 19 | Complex pending fixture is structurally valid; unresolved legacy fields remain open. |

## Negative Fixtures

| Report | Decision | Expected Error Codes |
|---|---:|---|
| `negative_missing_required_file_validation_001.json` | fail | `PV001_REQUIRED_FILE_MISSING` |
| `negative_broken_binding_validation_001.json` | fail | `LGV004_GEOMETRY_PATH_BROKEN` |
| `negative_acis_pointer_as_geometry_id_validation_001.json` | fail | `GRV004_ACIS_POINTER_USED_AS_STABLE_ID` |
| `negative_transient_index_only_validation_001.json` | fail | `GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID` |
| `negative_forged_evidence_validation_001.json` | fail | `EV001_EVIDENCE_ID_NOT_FOUND`, `PRV004_EVIDENCE_REF_BROKEN` |
| `negative_pending_field_marked_confirmed_validation_001.json` | fail | `EV003_PENDING_MARKED_CONFIRMED`, `EV004_CONFIRMED_WITHOUT_EVIDENCE` |
| `negative_detail_rsd_mismatch_validation_001.json` | fail | `DMV002_RSD_ID_MISMATCH` |
| `negative_half_written_package_validation_001.json` | fail | `PV004_SAVE_TRANSACTION_INCOMPLETE`, `TXV001_TEMP_PACKAGE_INVALID`, `TXV006_HALF_WRITTEN_PACKAGE_OPENED_AS_FORMAL` |
| `negative_missing_step_diff_validation_001.json` | fail | `GRV006_STEP_DIFF_MISSING` |

## Gate Decision

This run moves `GAP-DEV-001` forward because validator code and report files now exist.

It does not close `GAP-DEV-001` because the following are still missing:

- Save/Open transaction tests in the Qt6 application.
- Binding repair workflow.
- Real STEP multi-run diff.
- Generated application-level validation reports during Phase 1 initialization.

It does not close `GAP-DEV-007` because readiness is still `M1-Prep`, not `M1-Formal-Ready`.
