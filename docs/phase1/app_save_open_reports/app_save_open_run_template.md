# Qt6 Application Save/Open Run Template

## Run Metadata

- Report ID:
- Date:
- App version:
- Schema version:
- Runner:
- Evidence ID: `E-DEV-018`
- Scope: Qt6 application Save/Open gate, not standalone transaction gate

## Cases

| Case ID | Command | Input | Expected | Actual | Decision | Report |
|---|---|---|---|---|---|---|
| ASO-001 | Project.OpenTsRebar | Fixture A | OpenedFormal | | | |
| ASO-002 | Project.OpenTsRebar | Fixture B | OpenedWarning / warning-only | | | |
| ASO-003 | Project.OpenTsRebar | Fixture C | Warning or repairRequired, not fully confirmed | | | |
| ASO-004 | Project.OpenTsRebar | missing required file | OpenBlocked | | | |
| ASO-005 | Project.Save | broken binding candidate | SaveFailed, old hash preserved, dirty kept | | | |
| ASO-006 | Project.Save | valid dirty package | SaveSucceeded, dirty cleared, audit exists | | | |
| ASO-007 | Project.Save | injected failure after temp write | old hash preserved, failure report exists | | | |
| ASO-008 | Binding.Repair.Begin | broken binding + candidate STEP | preview only, no package write | | | |
| ASO-009 | Binding.Repair.Apply | confirmed preview | apply then Save transaction | | | |
| ASO-010 | Binding.Repair.Apply | ambiguous candidate | unresolved, no silent resolved | | | |

## Required Evidence

- Validation reports:
- App save/open JSON report:
- Binding repair preview:
- Binding repair run:
- Old package hash before failed save:
- Old package hash after failed save:
- Dirty before failed save:
- Dirty after failed save:
- STEP selection report used by repair:

## Decision

- Qt6 application Save/Open gate result:
- Binding Repair gate result:
- M1-Formal candidate: yes/no
- Remaining blockers:

