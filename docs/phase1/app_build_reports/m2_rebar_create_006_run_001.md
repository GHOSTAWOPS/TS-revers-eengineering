# M2-RebarCreate-006 Run 001

todoId = TODO-076
phase = M2-RebarCreate-006
evidenceIds = E-IDA-046, E-DEV-098
decision = line-group-old-ui-prompt-static-stop-point-p0

## Summary

本轮完成 `TODO-076 / 线配筋旧 UI 失败提示与状态栏口径证据 P0`。

IDA MCP 已确认 `sgroupbarline` handler / secondary helper 内没有直接中文失败提示字符串。旧公共创建链只收窄到 ACIS outcome / part state 公共路径，以及 `Input_float / Dialog #383` 公共浮点输入窗。`Dialog #383` 不能等同于旧线配筋主参数窗口字段清单。

本轮没有改线配筋业务算法，没有启动旧图石，没有安装 HASP。

## Changed Files

```text
02_界面窗口参数矩阵.md
03_IDA命令证据.md
08_开发命令契约.md
11_需求证据追溯矩阵.md
35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md
99_缺口和待确认项.md
114_M2-RebarCreate-006线配筋旧UI失败提示与状态栏口径证据P0实现记录.md
docs/phase1/app_build_reports/m2_rebar_create_006_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_006_run_001.json
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
todo.csv
46_CSE_v2Goal执行目标与Todo说明.md
00_总览.md
34_Phase1ReadinessGate实际运行记录.md
```

## IDA Evidence

```text
database = visualts_todo076
hexrays_ready = true

checked functions:
  sub_1404DE720
  sub_14054B410
  sub_1404D10C0
  sub_14058B8D0
  sub_1404F5120
  sub_1406B7FE0
  sub_1406B8140
  sub_14054C720
  sub_14054C760

string sweep:
  线配筋|请选择|选择.*边|边.*选择|钢筋组|状态栏|失败|参数|距离
  matches = 0
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
occtDirectRebarRewrite = false
parentRebarMigrated = false
rebarGroupCreatorAlgorithmChanged = false
lineGroupParameterDialogChanged = false
golden = not_started
autocadL2 = not_run
oldStatusPromptsConfirmed = false
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
  -> Ran 36 tests in 0.447s, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict --report-id readiness_gate_run_todo_076
  -> decision=M1-Formal-Ready, errorCount=0, warningCount=0, passCount=84/84

ctest --test-dir .\app\build --output-on-failure
  -> 100% tests passed, 0 tests failed out of 21, total=206.57 sec

rg -n "TopoDS_|AIS_|BRep|TopAbs_|OpenCASCADE|opencascade" .\app\src\domain\rebar .\app\src\command
  -> no matches

git diff --check
  -> exit 0

xhigh readonly review
  -> Assessment=needs_fix, Critical=none, Important fixed:
     1. 回填 xhighReview 结论。
     2. 将只读 review 待完成状态纳入 done-report pending marker。
     3. 增加 TODO-076 pending review 负例 gate 测试。
     4. 修正 99 中 TODO-076 当前主线残留。
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
nextTodo = TODO-077
action = LineGroup old VisualTS runtime capture checklist and artifact gate P0
```

TODO-077 只准备旧图石运行确认清单，不自动进入完整线配筋算法。
