# STEP Selection Gate Summary 001

- Date: 2026-06-05
- Evidence: `E-DEV-017`
- Runner: `tools/step_selection_gate/run_step_selection_gate.py`
- Probe: parent-reference `visualts_step_probe --refs-json`
- Scope: OCCT STEP import stable `selection-v1` refs; not Qt6 application Save/Open

## Results

| Sample | Runs | Copy-path run | Decision | Face refs | Edge refs | Vertex refs | Finding |
|---|---:|---|---|---:|---:|---:|---|
| `test_stp` | 5 | yes | pass | 75 / 1.000000 / dup0 | 195 / 1.000000 / dup0 | 130 / 1.000000 / dup0 | Unique OCCT subshape map plus `step-shape-fingerprint/v1` removes raw edge/vertex duplicate noise. |
| `cc001_bspline_edge` | 5 | yes | pass | 0 / 1.000000 / dup0 | 1 / 1.000000 / dup0 | 2 / 1.000000 / dup0 | BSpline edge stable. |
| `cc002_bezier_edge` | 5 | yes | pass | 0 / 1.000000 / dup0 | 1 / 1.000000 / dup0 | 2 / 1.000000 / dup0 | Bezier edge stable. |
| `cc003_bspline_length` | 5 | yes | pass | 0 / 1.000000 / dup0 | 1 / 1.000000 / dup0 | 2 / 1.000000 / dup0 | BSpline length sample stable. |
| `cc004_hostref_source_curve` | 5 | yes | pass | 0 / 1.000000 / dup0 | 1 / 1.000000 / dup0 | 2 / 1.000000 / dup0 | Source curve preservation sample stable. |
| `cc005_sampling_policy` | 5 | yes | pass | 0 / 1.000000 / dup0 | 1 / 1.000000 / dup0 | 2 / 1.000000 / dup0 | Sampling policy sample stable. |
| `tushi_rebar_123_stp` | 5 | yes | pass | 3016 / 1.000000 / dup0 | 4524 / 1.000000 / dup0 | 3016 / 1.000000 / dup0 | Old Tushi exported steel STEP from downstream stilling basin SFL; OCCT import reports 754 solids and stable refs. |

## Decision

`GAP-DEV-002` can move from `P0` to `P1 residual`.

The current Phase 1 STEP selection gate is now backed by real OCCT runs for all
seed samples listed above. It does not close future real-project sample risk,
and it does not replace Qt6 application Save/Open or binding repair workflow.

## Artifacts

- `docs/phase1/step_selection/test_stp/step_selection_diff.md`
- `docs/phase1/step_selection/test_stp/step_selection_diff.json`
- `docs/phase1/step_selection/test_stp/step_selection_run_r*.json`
- `docs/phase1/step_selection/cc001_bspline_edge/step_selection_diff.md`
- `docs/phase1/step_selection/cc001_bspline_edge/step_selection_diff.json`
- `docs/phase1/step_selection/cc001_bspline_edge/step_selection_run_r*.json`
- `docs/phase1/step_selection/cc002_bezier_edge/`
- `docs/phase1/step_selection/cc003_bspline_length/`
- `docs/phase1/step_selection/cc004_hostref_source_curve/`
- `docs/phase1/step_selection/cc005_sampling_policy/`
- `docs/phase1/step_selection/tushi_rebar_123_stp/`

## Remaining Work

- Run this gate again when real project STEP samples replace the seed samples.
- Keep fingerprint fields in the future Qt6 application binding implementation.
- Keep ambiguous or missing geometry refs blocked from formal binding.
