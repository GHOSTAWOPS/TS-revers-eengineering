# M2-RebarCreate-005 线配筋选择预检与参数窗口顺序对齐 P0 实现记录

todoId = TODO-075
phase = M2-RebarCreate-005
evidenceIds = E-DEV-097

## 目标

本轮只完成 `TODO-075 / 线配筋选择预检与参数窗口顺序对齐 P0`。

目标是让 `Rebar.Create.LineGroup` 在打开 `LineGroupParameterDialog` 前，先完成最小选择预检：

```text
无选择
  -> 不打开参数窗口
  -> 不执行 command handler
  -> 不污染 SteelData
  -> 不刷新 AIS

非 edge 选择
  -> 不打开参数窗口
  -> 不执行 command handler
  -> 不污染 SteelData
  -> 不刷新 AIS

有效 edge 选择
  -> 打开参数窗口
  -> 保留 TODO-074 的默认参数 / 用户修改参数传递能力
```

本轮不是完整线配筋算法，也不是旧 UI 状态栏提示闭合。

## 证据边界

本轮没有新增 IDA MCP 结论，原因是 `TODO-071 / E-IDA-045` 已经给出本节点需要的旧入口顺序证据：

```text
sub_1404DE720:
  selection count == 1
  -> sub_1405C6820 / sub_1405DA020 选择对象 gate
  -> child count >= 2
  -> 奇数索引 child 抽取到 ENTITY_LIST
  -> 4 个端点距离候选
  -> 10.0 初始最小距离候选
  -> sub_1404D10C0(...)
```

因此本轮只把 Qt6 命令入口的最小顺序拉回“先看选择，再开参数窗口”。旧状态栏提示、失败提示、旧参数 Dialog 字段、选择对象业务名和完整 `ENTITY_LIST` 语义继续保留到 `GAP-UI-REB-001 / GAP-IDA-002`。

## 已实现

`MainWindow` 新增只读预检 helper：

```text
lineGroupSelectionPreflightForCommand(QString* errorMessage)
```

预检规则：

```text
m_viewer->currentSelectionRef() 为空
  -> 失败：线配筋需要先选择边

current.shapeKind != LegacyShapeKind::Edge
  -> 失败：线配筋当前只接受边选择

否则通过
```

`executeCommand(RebarLineCreate)` 顺序调整为：

```text
Rebar.Create.LineGroup
  -> lineGroupSelectionPreflightForCommand()
  -> LineGroupParameterDialog(nextLineGroupParameters())
  -> Accepted: m_lineGroupHandler->setParameters(...)
  -> CommandRegistry::execute(RebarLineCreate)
  -> 成功后 displayCreatedLineGroup(...)
```

预检失败时只更新状态栏，不进入参数窗口、不执行 handler。

## TDD

红灯记录：

```text
新增无选择 / 非 edge 不打开参数窗口断言后，
旧实现会先打开 LineGroupParameterDialog，
失败消息为：
  line group command must not open parameter dialog without selection
```

绿灯记录：

```text
line_group_display_smoke_123 = pass
```

## 测试覆盖

扩展 `line_group_display_smoke_tests`：

```text
1. 无选择时触发 Rebar.Create.LineGroup：
   - 参数窗口不得打开
   - SteelData group 数量不变
   - AIS displayedRebarShapeCount 不变

2. 选中真实 123.stp face 后触发 Rebar.Create.LineGroup：
   - 参数窗口不得打开
   - SteelData group 数量不变
   - AIS displayedRebarShapeCount 不变

3. 选中真实 123.stp edge 后：
   - 取消参数窗口不污染 SteelData / 不刷新 AIS
   - 确认默认参数可创建并显示
   - 修改参数仍能进入最新 SteelBarGroup
```

## 验证

```text
targetedBuild = pass; cmake --build app/build --target line_group_display_smoke_tests; ninja: no work to do
targetedTest = pass; line_group_display_smoke_123 1/1 pass; 78.00 sec
fullBuildDebug = pass; vcvars64 + cmake --build app/build --config Debug
ctest = pass; 21/21 pass; 206.82 sec
readinessGateUnit = pass; 35/35 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ / OpenCASCADE / opencascade matches
gitDiffCheck = pass; no whitespace errors; CRLF/LF warnings only for 11_需求证据追溯矩阵.md and todo.csv
xhighReview = allow_commit; Hubble; no Critical / Important / Minor; agent closed
```

## 边界

本轮允许修改：

```text
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/tests/integration/line_group_display_smoke_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
文档 / run report / todo.csv
```

本轮没有做：

```text
没有改 RebarGroupCreator 算法。
没有迁入父目录 rebar 业务。
没有让 domain/rebar 或 command handler 依赖 OCCT/AIS。
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
TODO-076 / M2-RebarCreate-006
线配筋旧 UI 失败提示与状态栏口径证据 P0
```

原因：

```text
TODO-075 已把“先选择预检，再打开参数窗口”的最小顺序对齐。
但旧状态栏提示、失败提示、参数窗口字段和选择对象业务名仍未确认。
下一轮应优先用 IDA MCP 或旧图石运行确认补证，
不要直接跳到完整线配筋算法。
```

## xhigh Review

只读代理 `Hubble` 结论：

```text
Verdict = ready_to_commit
Critical = none
Important = none
Minor = none
Route Drift = none
Agent closed = true
```

复核重点：

```text
1. MainWindow 先执行 lineGroupSelectionPreflightForCommand，失败直接 return。
2. line_group_display_smoke_tests 覆盖无选择、face、edge cancel、edge success 和参数编辑。
3. 未修改 RebarGroupCreator。
4. domain/rebar 和 command 无 OCCT/AIS/BRep/TopAbs 泄漏。
5. todo.csv 只有一个 next，且为 TODO-076。
```
