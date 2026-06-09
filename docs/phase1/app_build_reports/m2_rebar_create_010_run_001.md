# M2-RebarCreate-010 Run 001

todoId = TODO-080
phase = M2-RebarCreate-010
evidenceIds = E-IDA-045, E-IDA-047, E-DEV-102
decision = done_core_gate_diagnostic_aligned

## Summary

本轮完成 `TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0` 的代码与测试主体。

核心变化：

```text
RebarGroupCreationRequest 新增 LegacyPublicCreateGateSnapshot。

RebarGroupCreator 现在在读取几何前先执行旧 sub_1404D10C0 已确认 gate：
  objB present
  objA present
  sub_1405F25F0(objA) >= 3
  ENTITY_LIST count >= 1
  distanceA >= 0.002

createdFromParameters 记录上述 gate 的 raw evidence 字段。
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
rebarGroupCreatorTarget = pass
defaultCTest = pass
readinessGateUnit = pass
readinessGateStrict = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = needs_fix_then_fixed_by_main_flow
```

Fresh verification commands already run:

```text
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" && cmake --build .\app\build --target rebar_group_creator_tests && ctest --test-dir .\app\build -R rebar_group_creator_tests --output-on-failure"
  -> pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" && cmake --build .\app\build && ctest --test-dir .\app\build --output-on-failure"
  -> 21/21 tests passed, 0 failed, total=206.12 sec

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> first run failed only because this report still had pending markers;
     xhigh reviewer confirmed the same issue;
     pending markers were fixed by main flow and gate rerun passed 44/44

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_080
  -> M1-Formal-Ready, 84/84 pass, 0 errors, 0 warnings

ctest --test-dir .\app\build --output-on-failure
  -> 21/21 tests passed, 0 failed, total=210.43 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

git diff --check
  -> pass; CRLF normalization warnings only

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> one next item: TODO-081

xhigh_todo080_final_reviewer
  -> Verdict: needs_fix
  -> Critical: none
  -> Important: run report still had pending markers
  -> Fixed by main flow in this report update
```

## Residual Gaps

```text
GAP-IDA-002 = old LineGroup selection object business name and runtime UI flow still open
GAP-IDA-007 = objA / objB / createdPayload / sub_1405D5670 full semantics still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-081
action = sub_1404D10C0 createdPayload and objA/objB field semantics static evidence P0
```

`TODO-081` 不应自动开始；本轮完成后停下复盘。
