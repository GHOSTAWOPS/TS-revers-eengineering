# Save/Open Transaction Gate Run 001

## Summary

- Date: 2026-06-05
- Runner: `tools/tsrebar_transaction_gate/transaction_gate.py`
- Result: `standalone-pass`
- Evidence: `E-DEV-016`

This run validates standalone Save/Open transaction semantics around the
`tsrebar` validator. It is not the final Qt6 application Save/Open test.

## Save Cases

| Scenario | Decision | Validation | Dirty | Guardrail |
|---|---:|---:|---:|---|
| `save_broken_binding` | SaveFailed | fail | `true -> true` | old package hash preserved |
| `save_fixture_b_single_group` | SaveSucceeded | warning-only | `true -> false` | current package replaced |

## Open Cases

| Package | Open State | Validation |
|---|---:|---:|
| `fixture_a_empty_step` | formalEdit | pass |
| `fixture_b_single_group` | warningOnly | warning-only |
| `fixture_c_pending_legacy` | warningOnly | warning-only |
| `negative/missing_required_file` | blocked | fail |
| `negative/half_written_package` | blocked | fail |

## Gate Decision

This run proves the standalone transaction gate can enforce:

- validator error does not overwrite the old package
- validator error does not clear dirty
- warning-only packages can open in warning mode
- broken packages open as blocked

It does not close `GAP-DEV-001` because these are still missing:

- Qt6 application Save/Open integration
- application-level rollback report
- binding repair workflow
- real STEP multi-run diff
