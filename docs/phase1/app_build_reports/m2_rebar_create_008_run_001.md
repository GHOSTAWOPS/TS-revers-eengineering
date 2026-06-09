# M2-RebarCreate-008 Run 001

todoId = TODO-078
phase = M2-RebarCreate-008
evidenceIds = E-DEV-100
decision = blocked_runtime_artifacts_missing

## Summary

本轮审计 `TODO-078 / 线配筋旧图石运行确认工件回填 P0`。

检查对象：

```text
docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/
```

当前目录只有模板文件：

```text
README.md
capture_notes.md
collect_hashes.ps1
```

没有发现真实旧图石运行工件：

```text
hashes.txt = missing
screenshots = missing
listing files = missing
output artifact hashes = missing
capture_notes.md populated fields = missing
```

## Decision

```text
TODO-078 = blocked
reason = waiting_for_user_provided_old_visualts_runtime_artifacts
```

本轮不把空模板外推成旧图石运行确认。

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
rebarGroupCreatorAlgorithmChanged = false
lineGroupParameterDialogChanged = false
occtDirectRebarRewrite = false
parentRebarMigrated = false
golden = not_started
autocadL2 = not_run
oldStatusPromptsConfirmed = false
oldDialogFieldsConfirmed = false
runtimeCaptureBackfilled = false
```

## Verification

```text
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
todoSingleNext = pass
gitDiffCheck = pass
xhighReview = not_required_docs_only
```

说明：

```text
本轮只改文档、run report 和 todo 状态。
未改代码、测试或构建脚本，所以 xhigh 只读 review 不强制。
```

Fresh verification commands:

```text
py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> Ran 40 tests, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_078_blocked
  -> decision=M1-Formal-Ready, errorCount=0, warningCount=0, passCount=84/84

ctest --test-dir .\app\build --output-on-failure
  -> 100% tests passed, 0 tests failed out of 21, total=208.54 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

Import-Csv .\todo.csv | Where-Object { $_.status -eq 'next' }
  -> TODO-079 only

git diff --check
  -> exit 0; CRLF normalization warnings only for 11_需求证据追溯矩阵.md and todo.csv
```

## Residual Gaps

```text
GAP-UI-REB-001 = old LineGroup dialog fields/defaults/units/gray state/status prompts still open
GAP-IDA-002 = old LineGroup selection object business name and runtime UI flow still open
GAP-IDA-007 = full objA / objB / createdPayload / sub_1405D5670 arg semantics still open
GAP-REB-C-002 = full line rebar creation, undo/dirty/runtime golden still open
```

## Next

```text
nextTodo = TODO-079
action = LineGroup old UI / Dialog static resource supplemental evidence P0
```

`TODO-079` 只做 IDA MCP / 静态资源补证，不替代 `TODO-078` 的真实旧图石运行确认。
