# M2-RebarCreate-001 Run 001

todoId = TODO-071
phase = M2-RebarCreate-001
evidenceIds = E-IDA-045, E-DEV-093
decision = sgroupbarline-entry-contract-p0-slice-prepared

## Summary

本轮完成 `TODO-071 / 线配筋生成旧逻辑证据与 P0 切片准备`。

本轮用 IDA MCP 重新确认 `sgroupbarline -> sub_1404DE720 -> sub_1404D10C0 -> sub_140451730 -> sub_1405D5670`，并把 `sgroupbarline` 入口契约写入当前 `RebarGroupCreator` P0 legacy raw / evidence。

## Changed Code

```text
app/src/domain/rebar/RebarGroupCreator.cpp
app/tests/unit/rebar_group_creator_tests.cpp
```

## Implemented

```text
line group createdFromParameters now records:
  sgroupbarline.selectionCount = 1
  sgroupbarline.selectionGate.structureCandidate = sub_1405C6820
  sgroupbarline.selectionGate.payloadCandidate = sub_1405DA020
  sgroupbarline.minimumInternalItemCount = 2
  sgroupbarline.oddIndexedEntityExtraction = child-index-1-3-5-...
  sgroupbarline.endpointDistanceCandidateCount = 4
  sgroupbarline.initialMinimumDistanceCandidate = 10.0
  sgroupbarline.publicCreateCall = sub_1404D10C0(...)

line group evidence now includes:
  E-IDA-045

line group unresolved fields now keep:
  sgroupbarline.selectionObjectType -> GAP-IDA-002
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
occtDirectRebarRewrite = false
parentRebarMigrated = false
detailWriterModified = false
golden = not_started
autocadL2 = not_run
domainRebarOccLeak = pass
```

## Verification

```text
idaMcp = pass; visualts_todo071_line_rebar opened VisualTS.exe.i64 and confirmed sgroupbarline table / handler / call chain
tddRed = pass; rebar_group_creator_tests failed before implementation with missing E-IDA-045 evidence
targetedBuild = pass; cmake --build app/build --target rebar_group_creator_tests --config Debug
targetedTest = pass; ctest -R rebar_group_creator_tests
fullBuildDebug = pass; cmake --build app/build --config Debug
ctest = pass; 18/18 pass; total 157.10 sec
domainRebarOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches
gitDiffCheck = pass
readinessGateUnit = pass; 31/31 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
xhighReview = needs_fix; important findings fixed by main agent; agent closed
```

## Residual Gaps

```text
GAP-IDA-002 = line selection object business name / old UI flow still needs runtime confirmation
GAP-IDA-007 = objA / objB / createdPayload / sub_1405D5670.arg4 still not fully named
GAP-REB-C-002 = full line rebar creation, command transaction, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-072
action = Rebar.Create.LineGroup command handler P0, bridge LegacySelection/parameters to RebarGroupCreator transaction
```
