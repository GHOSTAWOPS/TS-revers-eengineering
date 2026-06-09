# M2-RebarCreate-007 Run 001

todoId = TODO-077
phase = M2-RebarCreate-007
evidenceIds = E-DEV-099
decision = line-group-old-runtime-capture-checklist-p0

## Summary

本轮完成 `TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，
没有实现或修改线配筋算法。

核心结论：

```text
runtimeCaptureTemplate:
  docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/
    README.md
    capture_notes.md
    collect_hashes.ps1

checklist:
  no-selection click result
  wrong-object click result
  valid-object click result
  old main parameter dialog fields/defaults/units/disabled state
  Dialog #383 title/label/default/unit/OK/Cancel behavior if it appears
  status bar pane before/after
  model tree or output artifact before/after
  screenshots/listings/hashes/steps

rejectRules:
  menu screenshot alone is not runtime evidence
  screenshot without status bar or popup context cannot close prompt/status gaps
  Dialog #383 screenshot without title/label/default/button context is rejected
  new Qt6 P0 dialog screenshot is not old VisualTS evidence
  SFL/hash/steps are mandatory for runtime closure
  startup/license/HASP prompt alone is rejected
```

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
```

## Verification

```text
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
gitDiffCheck = pass
xhighReview = needs_fix_important_fixed
```

Fresh verification commands:

```text
py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> Ran 40 tests, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_077
  -> decision=M1-Formal-Ready, errorCount=0, warningCount=0, passCount=84/84

ctest --test-dir .\app\build --output-on-failure
  -> 100% tests passed, 0 tests failed out of 21, total=207.14 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

git diff --check
  -> exit 0; CRLF normalization warnings only for 11_需求证据追溯矩阵.md and todo.csv
```

xhigh readonly review:

```text
Assessment = needs_fix
Critical = none
Important fixed:
  1. TODO-077 done report had pending verification placeholders.
  2. The final-verification placeholder was not in done-report pending markers.

Fixes applied by main flow:
  1. Replaced TODO-077 pending verification placeholders with fresh verification results.
  2. Added the final-verification placeholder to `PENDING_REPORT_MARKERS`.
  3. Added TODO-077 pending final verification negative gate test.
  4. Closed reviewer agent after completion.
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
nextTodo = TODO-078
action = LineGroup old VisualTS runtime capture artifact backfill P0
```

TODO-078 只接收和核对真实旧图石运行工件，不自动进入完整线配筋算法。
