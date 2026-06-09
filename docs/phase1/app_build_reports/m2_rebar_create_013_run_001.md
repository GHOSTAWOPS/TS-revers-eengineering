# M2-RebarCreate-013 Run 001

todoId = TODO-083
phase = M2-RebarCreate-013
evidenceIds = E-IDA-022, E-IDA-048, E-DEV-104, E-DEV-105
decision = done_split_spline_trim_trace_aligned

## Summary

本轮完成 `TODO-083 / 线配筋公共创建 sub_1405D5670 split-spline-trim 等价切片 P0`。

核心结果：

```text
LegacySegmentCurveNormalizeTrace / Result
  -> 让 normalizeSegmentCurve 同时返回 curve 和 trace

createdFromParameters / legacyRaw
  -> 记录 sub_1405D5670.normalize.capabilityLevel
  -> 记录 api_entity_entity_distance / api_split_curve / api_curve_spline
  -> 记录 api_curve_spline.effectiveSampleCount
  -> 记录 sub_140580950.startTrim / endTrim
  -> 保留 sub_14059B980.groupMinimumDistanceTrimLoop = deferred-p0
  -> 保留 sub_1405BD0C0.backupWriteEdge = deferred-domain-model

MainWindow viewer reader
  -> 通过 OccLegacyGeometryAdapter 生成 P0 summary trace
  -> OCCT 仍隔离在 app/viewer reader 层，没有进入 domain/rebar
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
uiTextChanged = false
fullLineGroupAlgorithmImplemented = false
faceRebarImplemented = false
arcRebarExpanded = false
jointImplemented = false
excelImplemented = false
detailChanged = false
golden = not_started
occtDirectRebarRewrite = false
parentRebarMigrated = false
domainRebarOcctLeak = pass
```

## Verification

```text
targetedSmokeAfterFix = pass
defaultCTest = pass
readinessGateUnit = pass
readinessGateStrict = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = needs_fix_then_fixed_by_main_flow
```

Fresh verification commands:

```text
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build --target line_group_display_smoke_tests && ctest --test-dir .\app\build -R line_group_display_smoke_123 --output-on-failure"
  -> 1/1 pass, 78.46 sec

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build && ctest --test-dir .\app\build --output-on-failure"
  -> 21/21 pass, 204.69 sec

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> 53/53 pass

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_083
  -> pass after TODO-083 report and todo.csv mapping were added

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches, exit code 1 expected

git diff --check
  -> pass

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-084 only
```

## XHigh Readonly Review

```text
reviewer = xhigh_readonly_reviewer / Mencius
verdict = needs_fix
critical_1 = MainWindow viewer reader recorded length*50 sample count but called buildSplineFromPoints with minimumSplineSamples
critical_2 = docs marked TODO-083 done before todo.csv/run report/json were present
important_1 = implementation record used xhighReadonlyReview while gate contract expects xhighReview
important_2 = original tests covered fake domain trace but not viewer reader sample-count semantics
mainFlowFix =
  -> MainWindow now passes effectiveSplineSampleCount to buildSplineFromPoints and backfills adapter effective count
  -> line_group_display_smoke_123 now checks raw effectiveSampleCount against latest segment length * 50
  -> todo.csv now marks TODO-083 done and TODO-084 next
  -> run report / JSON / readiness mapping / docs were completed
  -> implementation record wording changed to xhighReview
agentClosed = true
```

## Residual Gaps

```text
GAP-IDA-002 = old LineGroup selection object business name and runtime UI flow still open
GAP-IDA-007 = full sub_1405D5670 topology mutation, group min-distance loop, backup/write edge and dirty parity still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-084
action = line group public create sub_1405D5670 group-min-distance / dirty-write gap slice P0
```

`TODO-084` 不应自动开始；本轮完成后停下复盘。
