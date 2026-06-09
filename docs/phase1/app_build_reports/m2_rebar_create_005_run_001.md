# M2-RebarCreate-005 Run 001

todoId = TODO-075
phase = M2-RebarCreate-005
evidenceIds = E-DEV-097
decision = line-group-selection-preflight-before-dialog-p0

## Summary

本轮完成 `TODO-075 / 线配筋选择预检与参数窗口顺序对齐 P0`。

`Rebar.Create.LineGroup` 现在会在打开参数窗口前先检查当前选择。无选择或非 edge 选择时不会打开 `LineGroupParameterDialog`，不会执行 handler，不污染 `SteelData`，也不刷新 AIS。有效 edge 选择时继续保留 TODO-074 的参数窗口和参数传递能力。

本轮没有新增 IDA MCP 结论，而是复用 `TODO-071 / E-IDA-045` 已确认的旧 `sub_1404DE720` 入口顺序证据。旧状态栏提示、失败提示和参数 Dialog 字段继续保留在 `GAP-UI-REB-001`。

## Changed Code

```text
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/tests/integration/line_group_display_smoke_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Implemented

```text
CommandId::RebarLineCreate
  -> lineGroupSelectionPreflightForCommand()
       no selection -> statusBar failure and return
       non-edge     -> statusBar failure and return
       edge         -> continue
  -> LineGroupParameterDialog(nextLineGroupParameters())
  -> Accepted:
       RebarLineGroupCommandHandler::setParameters(...)
       CommandRegistry::execute(RebarLineCreate)
       displayCreatedLineGroup(...)
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
occtDirectRebarRewrite = false
parentRebarMigrated = false
rebarGroupCreatorAlgorithmChanged = false
golden = not_started
autocadL2 = not_run
oldStatusPromptsConfirmed = false
```

## Verification

```text
targetedBuild = pass; cmake --build app/build --target line_group_display_smoke_tests; ninja: no work to do
targetedTest = pass; line_group_display_smoke_123 1/1 pass; 78.00 sec
directBuildWithoutVcvars = fail_environment; cl.exe could not find <type_traits>
fullBuildDebug = pass; cmd /c call vcvars64.bat && cmake --build app/build --config Debug
ctest = pass; 21/21 pass; 206.82 sec
readinessGateUnit = pass; 35/35 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ / OpenCASCADE / opencascade matches
gitDiffCheck = pass; no whitespace errors; CRLF/LF warnings only for 11_需求证据追溯矩阵.md and todo.csv
xhighReview = allow_commit; Hubble; no Critical / Important / Minor; agent closed
```

## Residual Gaps

```text
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/units/gray state/status prompts still open
GAP-IDA-002 = old single-selected-entity-list / selection object business name still open
GAP-IDA-007 = full objA / objB / createdPayload / sub_1405D5670 arg semantics still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-076
action = LineGroup old UI failure prompt and status bar evidence P0
```

## Final Review

```text
finalXhighAgent = Hubble
verdict = allow_commit
critical = none
important = none
minor = none
routeDrift = none
requiredFixBeforeCommit = none
agentClosed = true
```
