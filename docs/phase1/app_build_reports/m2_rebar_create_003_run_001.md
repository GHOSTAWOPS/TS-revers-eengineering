# M2-RebarCreate-003 Run 001

todoId = TODO-073
phase = M2-RebarCreate-003
evidenceIds = E-DEV-095
decision = line-group-ui-ais-feedback-p0

## Summary

本轮完成 `TODO-073 / 线配筋 UI 到 AIS 显示 P0`。

`Rebar.Create.LineGroup` 成功创建 `SteelBarGroup` 后，`MainWindow` 会通过 `RebarAisPresentationAdapter` 把 domain `SteelData` 映射为 AIS 显示项，并交给 `OccViewerWidget` 显示。

xhigh 只读 review 发现固定 group id 会让连续创建存在重复显示旧组风险；本轮已修复为每次执行线配筋前生成新的 P0 group/bar/segment id，并补连续两次创建的 smoke 断言。

## Changed Code

```text
app/CMakeLists.txt
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/src/command/RebarLineGroupCommandHandler.cpp
app/src/command/RebarLineGroupCommandHandler.h
app/src/presentation/occ/OccViewerWidget.cpp
app/src/presentation/occ/OccViewerWidget.h
app/tests/integration/line_group_display_smoke_tests.cpp
app/tests/unit/rebar_line_group_command_handler_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## Implemented

```text
CommandId::RebarLineCreate
  -> MainWindow refreshes unique P0 group/bar/segment ids
  -> RebarLineGroupCommandHandler
  -> SteelData append
  -> RebarAisPresentationAdapter::buildGroupPresentation
  -> OccViewerWidget::displayRebarPresentation
  -> AIS display count increments
```

失败路径：

```text
CommandStatus::Failed
  -> 不刷新 rebar AIS display
  -> displayedRebarShapeCount 不变
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
tddRed = pass; line_group_display_smoke_tests initially failed because OccViewerWidget lacked displayedRebarShapeCount
tddRedReviewFix = pass; rebar_line_group_command_handler_tests failed to link before RebarLineGroupCommandHandler::setParameters implementation
targetedBuild = pass; line_group_display_smoke_tests
targetedTest = pass; rebar_line_group_command_handler_tests + line_group_display_smoke_123
ctest = pass; 20/20 pass
readinessGateUnit = pass; 33/33 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches
parentRebarBusinessReference = pass; no forbidden parent rebar factory references in app code
gitDiffCheck = pass
xhighReview = needs_fix; critical review placeholder fixed; important duplicate group display fixed; route drift none
```

## Residual Gaps

```text
GAP-IDA-002 = old single-selected-entity-list / selection object business name still open
GAP-IDA-007 = full objA / objB / createdPayload / sub_1405D5670 arg semantics still open
GAP-REB-C-002 = full line rebar creation, parameter dialog, undo/dirty/runtime golden still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
```

## Next

```text
nextTodo = TODO-074
action = LineGroup parameter dialog P0
```
