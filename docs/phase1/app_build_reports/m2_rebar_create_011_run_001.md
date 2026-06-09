# M2-RebarCreate-011 Run 001

todoId = TODO-081
phase = M2-RebarCreate-011
evidenceIds = E-IDA-048, E-DEV-103
decision = done_static_payload_role_evidence

## Summary

本轮完成 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0`。

核心结论：

```text
createdObject + 104 = createdPayload
  -> 进入 sub_1405D5670
  -> 进入 runtime vtable display attach
  -> 进入 sub_1405E49D0 dirty 标记

createdObject + 112 = linkedModelRef / createdLinkRef
  -> 进入 sub_1406B6E20 / sub_1405F4ED0 / sub_1405F4A90 / sub_1405F5880 / sub_1406B2270
  -> 业务名仍低置信

sub_1405D5670 第 4 个 double = distanceA_4digit
  -> movsd xmm3, [var_1C8]
  -> var_1C8 = (int)(distanceA * 10000.0) / 10000.0

objA / objB 在线配筋和弧形组入口会交换角色，
不能写死成固定中文业务名。
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
uiTextChanged = false
codeChanged = false
fullLineGroupAlgorithmImplemented = false
faceRebarImplemented = false
arcRebarExpanded = false
jointImplemented = false
excelImplemented = false
detailChanged = false
golden = not_started
occtDirectRebarRewrite = false
parentRebarMigrated = false
domainRebarOcctLeak = not_applicable_no_app_code_change
```

## Verification

```text
idaMcp = pass
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = needs_fix_then_fixed_by_main_flow
xhighNotes = first readonly review found stale xhigh status and stale TODO-081 next wording; main flow fixed both and added TODO-081 report JSON contract guardrail
xhighFinalReview = allow_commit
```

Fresh verification commands:

```text
mcp__ida_pro_mcp.idb_list
  -> visualts_todo079 active

mcp__ida_pro_mcp.analyze_function sub_1404D10C0
  -> pass

mcp__ida_pro_mcp.analyze_function sub_140451730
  -> pass

mcp__ida_pro_mcp.decompile sub_1405D5670
  -> pass

mcp__ida_pro_mcp.decompile sub_1404DE720 / sub_1404DE110
  -> pass

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> pass

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_081
  -> pass

ctest --test-dir .\app\build --output-on-failure
  -> 21/21 pass, 208.96 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

git diff --check
  -> pass

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> one next item: TODO-082
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
nextTodo = TODO-082
action = line group public create roles DTO and raw evidence alignment P0
```

`TODO-082` 不应自动开始；本轮完成后停下复盘。
