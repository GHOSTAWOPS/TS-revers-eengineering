# M2-RebarCreate-012 Run 001

todoId = TODO-082
phase = M2-RebarCreate-012
evidenceIds = E-IDA-048, E-DEV-104
decision = done_roles_dto_raw_evidence_aligned

## Summary

本轮完成 `TODO-082 / 线配筋公共创建 roles DTO 与 raw evidence 对齐 P0`。

核心结果：

```text
LegacyPublicCreateRolesSnapshot
  -> 保存 objA / objB candidate role
  -> 保存 createdPayloadRef = createdObject+104
  -> 保存 linkedModelRef = createdObject+112
  -> 明确 linkedModelRefConfidence = low
  -> 明确 objAObjBMaySwapByEntryPoint = true

createdFromParameters / legacyRaw
  -> 写入 sub_1405D5670.distanceA4Digit
  -> 写入 createdPayloadRef / linkedModelRef / roleCandidate raw evidence

normalizeSegmentCurve
  -> 使用 distanceA_4digit 作为 unresolvedEndpointDistanceThreshold
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
targetedBuildAndTest = pass
targetedCTest = pass
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
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build --target rebar_group_creator_tests && .\app\build\rebar_group_creator_tests.exe"
  -> pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && ctest --test-dir .\app\build -R rebar_group_creator_tests --output-on-failure"
  -> 1/1 pass

cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul && cmake --build .\app\build && ctest --test-dir .\app\build --output-on-failure"
  -> 21/21 pass, 205.66 sec

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> pass after report pending markers were removed

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_082
  -> pass after report pending markers were removed

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches, exit code 1 expected

git diff --check
  -> pass

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-083 only
```

## XHigh Readonly Review

```text
reviewer = xhigh_readonly_reviewer / Kuhn
verdict = needs_fix_then_fixed_by_main_flow
critical = m2_rebar_create_012_run_001.md/json still contained unfinished verification markers while TODO-082 was already done
important = readiness gate correctly rejected the unfinished report; roles DTO remained raw evidence only
mainFlowFix = unfinished verification markers removed after final checks; readiness gate rerun required and passed
agentClosed = true
```

## Residual Gaps

```text
GAP-IDA-002 = old LineGroup selection object business name and runtime UI flow still open
GAP-IDA-007 = createdObject + 112 and object +80/+88/+96 exact semantics still open
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/status prompts still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-083
action = line group public create sub_1405D5670 split-spline-trim equivalence slice P0
```

`TODO-083` 不应自动开始；本轮完成后停下复盘。
