# M2-RebarCreate-002 Run 001

todoId = TODO-072
phase = M2-RebarCreate-002
evidenceIds = E-IDA-045, E-DEV-093, E-DEV-094
decision = line-group-command-handler-p0-wired

## Summary

本轮完成 `TODO-072 / 线配筋命令 handler P0：LegacySelection 到 RebarGroupCreator 事务接入`。

`Rebar.Create.LineGroup` 现在不再是 placeholder，而是通过 `RebarLineGroupCommandHandler` 接收当前选择、读取 legacy curve snapshot，并把结果交给 `RebarGroupCreator` 写入 `SteelData`。

## Changed Code

```text
app/CMakeLists.txt
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/src/command/LegacyUiCommandMap.cpp
app/src/command/RebarLineGroupCommandHandler.cpp
app/src/command/RebarLineGroupCommandHandler.h
app/src/presentation/occ/OccViewerWidget.cpp
app/src/presentation/occ/OccViewerWidget.h
app/tests/unit/command_registry_tests.cpp
app/tests/unit/rebar_line_group_command_handler_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Implemented

```text
CommandId::RebarLineCreate
  -> RebarLineGroupCommandHandler
  -> LegacySelectionRef single edge
  -> LegacyRebarGeometryReader
  -> RebarGroupCreator::createLineGroup
  -> SteelData append transaction

LegacyUiCommandMap
  -> Rebar.Create.LineGroup implementationState = Implemented
  -> inputSelectionTypes += edge-p0-surrogate
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
occtDirectRebarRewrite = false
parentRebarMigrated = false
domainRebarOccLeak = pass
commandLayerOccLeak = pass
golden = not_started
autocadL2 = not_run
```

## Verification

```text
tddRed = pass; command handler test target initially failed before RebarLineGroupCommandHandler existed
targetedBuild = pass; command_registry_tests, rebar_line_group_command_handler_tests, tsrebar_app
targetedTest = pass; command_registry_tests, rebar_line_group_command_handler_tests
smoke = pass; tsrebar_app --smoke
ctest = pass; 19/19 pass
readinessGateUnit = pass; 32/32 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches in app/src/domain/rebar or app/src/command
parentRebarBusinessReference = pass; no parent rebar factory references in protected LineGroup handler paths
gitDiffCheck = pass
xhighReview = allow_commit; Franklin reviewer reported no Critical/Important findings; minor null shared_ptr guard suggestion does not block commit; agent closed
```

## Residual Gaps

```text
GAP-IDA-002 = old single-selected-entity-list / selection object business name still open
GAP-IDA-007 = full objA / objB / createdPayload / sub_1405D5670 arg semantics still open
GAP-REB-C-002 = full line rebar creation, UI parameter dialog, AIS display refresh, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-073
action = LineGroup UI to AIS visible feedback P0
```
