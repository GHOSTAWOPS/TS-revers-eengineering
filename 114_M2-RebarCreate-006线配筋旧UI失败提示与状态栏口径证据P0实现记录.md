# M2-RebarCreate-006 线配筋旧 UI 失败提示与状态栏口径证据 P0 实现记录

todoId = TODO-076
phase = M2-RebarCreate-006
evidenceIds = E-IDA-046, E-DEV-098

## 目标

本轮只完成 `TODO-076 / 线配筋旧 UI 失败提示与状态栏口径证据 P0`。

目标是补齐 TODO-075 后仍未确认的旧证据：

```text
旧线配筋失败提示
旧状态栏 pane 文案
旧参数 Dialog 字段
旧运行点击流程
```

本轮是证据收窄，不实现完整线配筋算法。

## IDA MCP 证据

本轮使用 IDA MCP：

```text
database = visualts_todo076
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

补看的函数：

```text
sub_1404DE720
sub_14054B410
sub_1404D10C0
sub_14058B8D0
sub_1404F5120
sub_1406B7FE0
sub_1406B8140
sub_14054C720
sub_14054C760
```

字符串扫查：

```text
pattern = 线配筋|请选择|选择.*边|边.*选择|钢筋组|状态栏|失败|参数|距离
matches = 0
```

## 已确认

`sub_1404DE720 / sgroupbarline handler`：

```text
selection count != 1                         -> return 0
sub_1405C6820(selected) fail                 -> return 0
sub_1405DA020(selected+13) fail              -> return 0
child count < 2                              -> return 0
sub_1405BC270(obj+80) null                   -> return 0
first child +96 null                         -> return 0
```

该 handler 内没有直接中文失败提示字符串，也没有直接 `MessageBox / AfxMessageBox / SetPaneText` 类调用。

`sub_14054B410 / secondary helper`：

```text
ctx = sub_1406ED3C0(a1)
if ctx && selection_count >= 1:
  iterate selection list
  every selected entity must pass:
    sub_1405C6820
    sub_1405C6F90(entity) == 120
  all pass -> return 1
return 0
```

该 helper 更像命令可用性 / 状态检查，也没有直接提示字符串。

`sub_1404D10C0 / common creation core`：

```text
validate objA / objB / ENTITY_LIST / distance
distance < 0.002 -> fail
sub_14058B8D0(...)
api_bb_begin
ACISExceptionCheck("API")
sub_140451730(...)
sub_1405D5670(...)
problems_list_prop::process_result(...)
sub_1406B7FE0(...)
```

该公共链没有线配筋专属中文失败提示，只进入 ACIS outcome / part state 公共路径。

`Input_float / Dialog #383`：

```text
sub_14058B8D0
  -> sub_1404F5120
  -> CDialog::CDialog(..., 0x17F, ...)

0x17F = 383
source = e:\tushi3d\dam\tool\input_float.cpp
dialog + 304 = double value
dialog + 320 = copied title string
dialog + 328 = copied label string
```

## 工程结论

当前能高置信写入：

```text
旧 sgroupbarline handler 没有直接失败提示字符串。
旧 secondary helper 没有直接失败提示字符串。
旧 common creation core 没有线配筋专属提示字符串。
common creation core 会打开公共 Input_float / Dialog #383。
Dialog #383 不是旧线配筋主参数窗口字段清单。
```

当前不能写入：

```text
旧图石线配筋失败提示已确认。
旧图石线配筋状态栏 pane 文案已确认。
旧图石线配筋主参数窗口字段已确认。
当前 LineGroupParameterDialog 字段等于旧图石字段。
当前 P0 状态栏文案等于旧图石真值。
```

## 文档更新

本轮更新：

```text
02_界面窗口参数矩阵.md
03_IDA命令证据.md
08_开发命令契约.md
11_需求证据追溯矩阵.md
35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md
99_缺口和待确认项.md
```

新增：

```text
E-IDA-046
E-DEV-098
```

## 边界

本轮没有做：

```text
没有改 RebarGroupCreator 算法。
没有改 LineGroupParameterDialog 字段。
没有改 MainWindow UI 文案。
没有迁入父目录 rebar 业务。
没有让 domain/rebar 依赖 OCCT/AIS。
没有实现面配筋、弧筋、接头、Excel、Detail 或 golden。
没有自动启动旧图石。
没有安装 HASP 或修改系统目录。
```

## 验证

```text
readinessGateUnit = pass
readinessGateStrict = pass
defaultCTest = pass
domainRebarCommandOCCLeak = pass
gitDiffCheck = pass
xhighReview = needs_fix_important_fixed
```

验证命令：

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

最终验证结果见：

```text
docs/phase1/app_build_reports/m2_rebar_create_006_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_006_run_001.json
```

## 下一步

建议下一步进入：

```text
TODO-077 / M2-RebarCreate-007
线配筋旧图石运行确认清单与工件门禁 P0
```

运行确认应至少覆盖：

```text
1. 无选择点击线配筋：截图弹窗 / 状态栏。
2. 选择 face 或错误对象点击线配筋：截图弹窗 / 状态栏。
3. 选择有效对象点击线配筋：截图参数窗口、状态栏、是否出现 Dialog #383。
4. Dialog #383 若出现：记录标题、标签、默认值、单位、确认 / 取消行为。
5. 参数窗口若出现：记录字段、默认值、单位、灰显状态、确认 / 取消行为。
```
