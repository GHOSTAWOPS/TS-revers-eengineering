# M2-RebarCreate-004 线配筋参数窗口 P0 实现记录

todoId = TODO-074
phase = M2-RebarCreate-004
evidenceIds = E-DEV-096

## 目标

本轮只完成 `TODO-074 / 线配筋参数窗口 P0：旧 UI 字段占位、默认值和 handler 参数接入`。

目标是让 `Rebar.Create.LineGroup` 不再只能使用固定 P0 参数，而是先通过 Qt6 参数窗口接收用户输入，再把参数传给 `RebarLineGroupCommandHandler`。

本轮不是最终 UI，也不是完整旧图石线配筋算法。

## IDA 复核

本轮用 IDA MCP 重新检查 `VisualTS.exe.i64`：

```text
idb_open = visualts
find_regex = sgroupbarline / sgroupbararc / 线配筋 / 配筋 / 钢筋信息 / 结构信息 / 直径 / 间距 / 保护层 / 弯钩
analyze_function = sub_1404DE720
analyze_function = sub_1404D10C0
decompile = sub_1404DE720 / sub_1404D10C0
```

结果：

```text
1. 字符串直接命中 sgroupbarline / sgroupbararc。
2. 没有直接命中可证明线配筋参数 Dialog 的中文字段字符串。
3. sub_1404DE720 继续表现为：
   selection count == 1
   -> sub_1405C6820 / sub_1405DA020 选择对象 gate
   -> child count >= 2
   -> 奇数索引 child 抽取到 ENTITY_LIST
   -> 4 个端点距离候选
   -> 10.0 初始最小距离候选
   -> sub_1404D10C0(entityList,objA,objB,minDistance,selectedEndpointDistance,flag)
4. sub_1404D10C0 继续表现为公共创建链，不是参数 Dialog 入口。
```

因此本轮参数窗口字段只能标为 P0 占位，继续追溯到 `GAP-UI-REB-001`。不能写成旧图石线配筋参数窗口字段、默认值、单位或灰显状态已经闭合。

## 已实现

新增 `LineGroupParameterDialog`：

```text
直径       -> line_group_diameter_spin
间距       -> line_group_interval_spin
数量       -> line_group_bar_count_spin
级别       -> line_group_steel_level_combo
起点距离   -> line_group_distance_a_spin
终点距离   -> line_group_distance_b_spin
确定/取消  -> line_group_button_box
```

这些字段来自已确认的配筋字段族和当前 handler 参数 DTO，只是 P0 输入壳。

`MainWindow` 新增流程：

```text
Rebar.Create.LineGroup action
  -> LineGroupParameterDialog(nextLineGroupParameters())
  -> Accepted: m_lineGroupHandler->setParameters(...)
  -> CommandRegistry::execute(RebarLineCreate)
  -> 成功后 displayCreatedLineGroup(...)

Canceled:
  -> 不执行 command handler
  -> 不修改 SteelData
  -> 不刷新 AIS display
```

`RebarLineGroupCommandHandler` 和 `domain/rebar` 未引入 OCCT/AIS 细节。

## TDD

红灯：

```text
cmake --build build --target line_group_parameter_dialog_tests line_group_display_smoke_tests

失败原因：
  src/ui/LineGroupParameterDialog.cpp 不存在。
```

绿灯：

```text
line_group_parameter_dialog_tests = pass
line_group_display_smoke_123 = pass
```

## 测试覆盖

新增 `line_group_parameter_dialog_tests`：

```text
1. 默认参数能从 dialog 暴露。
2. 用户修改直径、间距、数量、级别、起点距离、终点距离后能映射回参数 DTO。
3. 直径 / 间距字段显示 mm 单位。
```

扩展 `line_group_display_smoke_tests`：

```text
1. 未选择对象时弹参数窗口并确定，handler 失败，AIS 不刷新。
2. 选中真实 123.stp edge 后取消参数窗口，SteelData 不变，AIS 不刷新。
3. 选中真实 123.stp edge 后确定默认参数，可创建并显示。
4. 第二次成功创建时修改参数，最新 SteelBarGroup 的 diameter / interval / barCount / steelLevel 使用用户输入。
```

## 验证

```text
targetedBuild = pass; line_group_parameter_dialog_tests + line_group_display_smoke_tests
targetedTest = pass; line_group_parameter_dialog_tests + line_group_display_smoke_123
fullBuildDebug = pass; cmake --build app/build --config Debug
ctest = pass; 21/21 pass
readinessGateUnit = pass; 33/33 pass before gate mapping, 34/34 pass after mapping
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches in app/src/domain/rebar or app/src/command
gitDiffCheck = pass
xhighReview = allow_commit; no Critical / Important
```

## xhigh Review

只读代理 `Erdos` 初审结论：

```text
Verdict = allow_commit
Critical = none
Important = none
Route Drift = none
```

补齐 `46 / 99` 当前 next 口径后，最终只读复审代理 `Nietzsche` 结论：

```text
Verdict = allow_commit
Critical = none
Important = none
Route Drift = none
Required Fix Before Commit = none
```

Minor / residual：

```text
1. 取消参数窗口会消耗一次 P0 sequence id；不污染 SteelData，但后续事务/undo 阶段可改成确认后再分配最终 id。
2. steelDataForInspection() 是 const P0 测试探针；后续可收窄为只读统计或最后创建组摘要。
3. LineGroupParameterDialog 直接 include handler 头只是为参数 DTO；后续可拆独立 DTO 头。
4. 当前 P0 是先弹参数窗再由 handler 校验选择；IDA 证据里的 sub_1404DE720 是先校验选择集。该行为差异已留给 TODO-075 对齐。
```

## 边界

本轮允许修改：

```text
app/src/ui/LineGroupParameterDialog.*
app/src/app/MainWindow.*
app/tests/unit/line_group_parameter_dialog_tests.cpp
app/tests/integration/line_group_display_smoke_tests.cpp
app/CMakeLists.txt
```

本轮没有做：

```text
没有迁入父目录 rebar 业务。
没有让 domain/rebar 依赖 OCCT/AIS。
没有实现面配筋、弧筋、接头、Excel、Detail 字段扩张或 golden。
没有自动启动旧图石。
没有安装 HASP 或修改系统目录。
```

## 明确不证明

```text
不证明旧线配筋参数窗口字段、默认值、单位、灰显状态已经 1:1。
不证明旧状态栏提示、失败提示、undo/dirty 已闭合。
不证明完整 ENTITY_LIST 选择对象语义。
不证明完整 sub_1405D5670 等价。
不证明完整线配筋、弧筋、面筋、接头、统计、出图或 golden。
```

## 下一步

建议下一步进入：

```text
TODO-075 / M2-RebarCreate-005
线配筋选择预检与参数窗口顺序对齐 P0
```

原因：

```text
IDA 证据显示旧 sub_1404DE720 先校验 selection count / 选择对象 gate，
当前 P0 为先弹参数窗口，再由 handler 校验选择。
下一轮只修 UI 命令顺序和可测事务边界，
不进入完整线配筋算法。
```
