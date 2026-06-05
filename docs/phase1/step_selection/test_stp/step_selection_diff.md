# STEP Selection Diff: test_stp

- Source STEP: `/mnt/c/users/ghost/desktop/reverse_engineering/test.stp`
- Runner: `tools/step_selection_gate/run_step_selection_gate.py`
- Probe: `visualts_step_probe --refs-json`
- Decision: `pass`

## Stable Ref Comparison

| Kind | Baseline count | Min match rate | Duplicate count | Mismatched runs |
|---|---:|---:|---:|---|
| faces | 75 | 1.000000 | 0 | - |
| edges | 195 | 1.000000 | 0 | - |
| vertices | 130 | 1.000000 | 0 | - |

## Boundary

- This is real OCCT STEP import evidence for the listed sample.
- It does not close Qt6 application Save/Open or binding repair gaps.
- Complex project-level samples still need the same gate before GAP-DEV-002 can be closed.
