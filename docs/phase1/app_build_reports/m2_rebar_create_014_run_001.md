# M2-RebarCreate-014 Run 001

todoId = TODO-084
phase = M2-RebarCreate-014
evidenceIds = E-IDA-022, E-IDA-048, E-IDA-049, E-DEV-105, E-DEV-106
decision = done_group_min_dirty_gap_contract

## Summary

本轮完成 `TODO-084 / 线配筋公共创建 sub_1405D5670 group-min-distance / dirty-write gap 切片 P0`。

核心结果：

```text
LegacySegmentCurveNormalizeTrace
  -> 继续保留 split / spline / trim trace
  -> 新增 group-min / backup-write / dirty gap contract

createdFromParameters / legacyRaw
  -> 记录 sub_14059B980.groupMinimumDistanceTrimLoop observed/deferred
  -> 记录 api_entity_point_distance = deferred-acis-group-list
  -> 记录 thresholdDistanceA4Digit
  -> 记录 start/end endpoint probe 和 iteration budget
  -> 记录 sub_1405BD0C0 ENTITY::backup + entity+72 write edge = observed-deferred
  -> 记录 sub_1404D10C0 post-create mutation order
  -> 记录 sub_1405E49D0.dirtyWrite = deferred-application-state

P0 guard
  -> false applied group-min / backup-write / dirty claim 会被拒绝
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
targetedRebarCreateTests = pass
targetedSmoke = pass
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
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build --target rebar_group_creator_tests rebar_line_group_command_handler_tests line_group_display_smoke_tests && ctest --test-dir .\app\build -R rebar_group_creator_tests --output-on-failure && ctest --test-dir .\app\build -R rebar_line_group_command_handler_tests --output-on-failure && ctest --test-dir .\app\build -R line_group_display_smoke_123 --output-on-failure"
  -> rebar_group_creator_tests pass
  -> rebar_line_group_command_handler_tests pass
  -> line_group_display_smoke_123 pass, 78.43 sec

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build && ctest --test-dir .\app\build --output-on-failure"
  -> 21/21 pass, 201.65 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches, exit code 1 expected

git diff --check
  -> pass

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-085 only
```

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> pass

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_084
  -> pass

xhigh readonly review
  -> needs_fix
  -> Important fixed by main flow:
     1. 99 stale TODO-084 next/current-mainline pointers corrected to TODO-085.
     2. backupWriteEdgeDeferred=false and dirtyWriteDeferred=false guard tests added.
  -> agent closed

## IDA Evidence

```text
mcp__ida_pro_mcp.idb_open VisualTS.exe.i64 -> visualts_todo084
mcp__ida_pro_mcp.analyze_batch sub_1405D5670 / sub_14059B980 / sub_1405BD0C0 / sub_1405E49D0
mcp__ida_pro_mcp.disasm sub_1405D5670
mcp__ida_pro_mcp.disasm sub_1404D10C0
```

Confirmed:

```text
sub_14059B980
  -> group+88 list
  -> node+72 entity
  -> api_entity_point_distance
  -> min distance return

sub_1405BD0C0
  -> ENTITY::backup
  -> entity+72 = edge

sub_1404D10C0
  -> sub_1405D5670
  -> sub_1405C7260
  -> vtable+0x1C8
  -> sub_1405E49D0
```

## Residual Gaps

```text
GAP-IDA-002 = old LineGroup selection object business name and runtime UI flow still open
GAP-IDA-007 = full sub_1405D5670 topology mutation, old group list, backup/write edge lifecycle and dirty parity still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-085
action = line group command success dirty/transaction state P0
```

`TODO-085` 不应自动开始；本轮完成后停下复盘。
