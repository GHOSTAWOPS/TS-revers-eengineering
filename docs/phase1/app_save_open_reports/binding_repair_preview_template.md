# Binding Repair Preview Template

## Metadata

- Report ID:
- Date:
- Source package:
- Current state:
- Candidate STEP:
- STEP selection diff:
- Evidence ID: `E-DEV-018`

## Broken Bindings

| Object ID | Role | Legacy Path | Old Geometry Path | Error Code | Current State |
|---|---|---|---|---|---|

## Candidate Mapping

| Object ID | Role | Old Ref | Candidate Ref | Match State | Evidence | Decision |
|---|---|---|---|---|---|

Allowed `Match State` values:

- `match`
- `ambiguous`
- `missing`
- `typeMismatch`
- `blocked`

Allowed `Decision` values:

- `apply`
- `keepUnresolved`
- `manualReview`
- `blocked`

## Guardrails

- Ambiguous candidates were not auto-applied:
- Missing candidates stayed unresolved:
- No silent rebind occurred:
- `legacyObject.raw` was not rewritten:
- Formal package was not written during preview:

## Decision

- Can apply:
- Must stay repairRequired:
- Follow-up GAP:

