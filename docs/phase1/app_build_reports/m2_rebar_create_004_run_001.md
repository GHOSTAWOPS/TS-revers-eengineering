# M2-RebarCreate-004 Run 001

todoId = TODO-074
phase = M2-RebarCreate-004
evidenceIds = E-DEV-096
decision = line-group-parameter-dialog-p0

## Summary

本轮完成 `TODO-074 / 线配筋参数窗口 P0`。

`Rebar.Create.LineGroup` 现在会先打开 `LineGroupParameterDialog`，用户确认后把直径、间距、数量、钢筋级别、起点距离和终点距离传给 `RebarLineGroupCommandHandler`。取消参数窗口不会执行 handler，不污染 `SteelData`，也不刷新 AIS。

IDA MCP 本轮没有找到可直接证明线配筋参数 Dialog 的字段字符串；`sub_1404DE720` 继续表现为选择对象 gate 到 `sub_1404D10C0` 创建链。因此本轮字段只按 P0 占位记录，继续保留 `GAP-UI-REB-001`。

## Changed Code

```text
app/CMakeLists.txt
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/src/ui/LineGroupParameterDialog.cpp
app/src/ui/LineGroupParameterDialog.h
app/tests/integration/line_group_display_smoke_tests.cpp
app/tests/unit/line_group_parameter_dialog_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Implemented

```text
CommandId::RebarLineCreate
  -> LineGroupParameterDialog(nextLineGroupParameters())
  -> Accepted:
       RebarLineGroupCommandHandler::setParameters(...)
       CommandRegistry::execute(RebarLineCreate)
       displayCreatedLineGroup(...)
  -> Rejected:
       return without command execution
       SteelData unchanged
       AIS display unchanged
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
oldUiFieldsConfirmed = false
```

## Verification

```text
tddRed = pass; initial build failed because LineGroupParameterDialog sources did not exist
targetedBuild = pass; line_group_parameter_dialog_tests + line_group_display_smoke_tests
targetedTest = pass; line_group_parameter_dialog_tests + line_group_display_smoke_123
fullBuildDebug = pass; cmake --build app/build --config Debug
ctest = pass; 21/21 pass; latest total 205.73 sec
readinessGateUnit = pass; 34/34 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches
gitDiffCheck = pass
xhighReview = allow_commit; no Critical / Important; final reviewer closed
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
nextTodo = TODO-075
action = LineGroup selection preflight and dialog ordering P0
```

## Final Review

```text
finalXhighAgent = Nietzsche
verdict = allow_commit
critical = none
important = none
routeDrift = none
requiredFixBeforeCommit = none
agentClosed = true
```
