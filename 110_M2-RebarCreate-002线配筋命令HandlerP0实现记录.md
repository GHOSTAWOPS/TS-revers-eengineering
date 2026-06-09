# M2-RebarCreate-002 线配筋命令 Handler P0 实现记录

todoId = TODO-072
phase = M2-RebarCreate-002
evidenceIds = E-IDA-045, E-DEV-093, E-DEV-094

## 目标

本轮只完成 `TODO-072 / 线配筋命令 handler P0：LegacySelection 到 RebarGroupCreator 事务接入`。

目标是把 `Rebar.Create.LineGroup` 从旧 UI 命令入口接到当前 `LegacySelectionRef + LegacyRebarGeometryReader + RebarGroupCreator` P0 事务边界。

本轮不是完整线配筋算法，也不是用 OCCT 直接重写钢筋。

## 已实现

新增 `RebarLineGroupCommandHandler`：

```text
SelectionProvider
  -> one LegacySelectionRef
  -> LegacyShapeKind::Edge
  -> LegacyRebarGeometryReader.curveSnapshot(...)
  -> RebarGroupCreator::createLineGroup(...)
  -> append SteelData groups / bars / segments / evidence / unresolvedLegacyFields
```

命令注册：

```text
CommandId::RebarLineCreate
  -> RebarLineGroupCommandHandler
  -> LegacyUiCommandMap implementationState = Implemented
```

UI 桥接：

```text
MainWindow
  -> OccViewerWidget.currentSelectionRef()
  -> ViewerLegacyRebarGeometryReader
  -> OccLegacyGeometryAdapter inside app/presentation bridge
```

其中 `ViewerLegacyRebarGeometryReader` 是 P0 过渡桥接：

```text
允许在 app / presentation 层把 viewer selection 转成 legacy rebar geometry。
不允许 domain/rebar 或 handler 直接依赖 TopoDS / AIS / BRep / TopAbs。
normalizeSegmentCurve P0 当前返回原 curve，不等价真实 trim/split 拓扑变更。
```

## 测试覆盖

新增 `rebar_line_group_command_handler_tests`：

```text
empty selection
  -> Failed
  -> 不查询 geometry
  -> 不污染 SteelData

wrong type face
  -> Failed
  -> 不查询 geometry
  -> 不污染 SteelData

valid edge
  -> Completed
  -> 创建 1 个 SteelBarGroup / SteelBar / SteelBarSegment
  -> 保留 createCommand = Rebar.Create.LineGroup
  -> 保留 legacyCommand = sgroupbarline
  -> 保留 selected edge stableId
  -> 进入 SegmentCurveNormalizer P0

geometry failure
  -> Failed
  -> 不污染 SteelData

normalizer failure
  -> Failed
  -> 不污染 SteelData
```

同步更新 `command_registry_tests`：

```text
Rebar.Create.LineGroup = Implemented
placeholder registry 不再给 RebarLineCreate 注册 NotImplemented handler
```

## 修改文件

```text
app/CMakeLists.txt
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/src/command/LegacyUiCommandMap.cpp
app/src/command/RebarLineGroupCommandHandler.cpp
app/src/command/RebarLineGroupCommandHandler.h
app/src/presentation/occ/OccViewerWidget.cpp
app/src/presentation/occ/OccViewerWidget.h
app/tests/unit/command_registry_tests.cpp
app/tests/unit/rebar_line_group_command_handler_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## TDD

红灯：

```text
rebar_line_group_command_handler_tests 初始阶段无法构建：
  missing command/RebarLineGroupCommandHandler.h
```

绿灯：

```text
rebar_line_group_command_handler_tests = pass
command_registry_tests = pass
tsrebar_app --smoke = pass
```

## xhigh 只读 Review

第一轮 xhigh 只读 review：

```text
reviewer = Copernicus
verdict = needs_fix
agentClosed = yes
```

发现与处理：

```text
Critical:
  handler 未接入正式 MainWindow
  -> 已修复：MainWindow 注册真实 LineGroup handler，placeholder 不覆盖

Important:
  lambda 捕获裸引用生命周期风险
  -> 已修复：handler 由 shared_ptr 持有

  缺少 geometry / normalizer failure 事务测试
  -> 已修复：新增失败不污染 SteelData 测试

  single-selected-entity-list 与 edge P0 surrogate 边界需写清
  -> 已修复：LegacyUiCommandMap 和文档明确 edge-p0-surrogate 只是 P0 代用

Minor:
  测试缺 <utility>
  -> 已修复

  sourceCurveIds.front() 前应确认非空
  -> 已修复
```

早期后续复审代理曾启动但超时，已关闭：

```text
Sagan = timed out and closed
Hilbert = timed out and closed
```

最终 xhigh 只读 review：

```text
reviewer = Franklin
verdict = allow_commit
agentClosed = yes
minor = registerRebarLineGroupCommandHandler does not guard null shared_ptr; current call site is valid and this does not block commit
xhighReview = allow_commit; previous Critical / Important / Minor fixed by main flow; earlier timed-out rereview agents were closed
```

## 验证

```text
targeted build = pass; command_registry_tests, rebar_line_group_command_handler_tests, tsrebar_app
targeted test = pass; command_registry_tests, rebar_line_group_command_handler_tests
smoke = pass; tsrebar_app --smoke
CTest = pass; 19/19 pass
readiness unit = pass; 32/32 pass
strict readiness gate = pass; M1-Formal-Ready; 84/84 pass
domain/rebar + command OCCT leak scan = pass
parent rebar business reference scan = pass
xhighReview = allow_commit; Franklin reviewer reported no Critical/Important findings; minor null shared_ptr guard suggestion does not block commit; agent closed
git diff --check = pass
```

## 明确不证明

```text
不证明完整旧线配筋算法。
不证明完整 sub_1405D5670 等价。
不证明旧 VisualTS 的 ENTITY_LIST 选择对象语义已经完整复刻。
不证明旧 UI 参数窗口、状态栏提示、失败提示已经运行确认。
不证明 AIS 中已经显示新生成钢筋。
不证明 undo / dirty / 保存 / golden。
不证明面配筋、弧筋、接头、Excel 或 AutoCAD L2。
```

## 下一步

建议下一步进入：

```text
TODO-073 / M2-RebarCreate-003
线配筋 UI 到 AIS 显示 P0：命令完成后显示创建的 SteelBarGroup
```

边界：

```text
继续只处理 Rebar.Create.LineGroup 的 UI 可见反馈。
不扩展到完整参数弹窗、面配筋、弧筋、接头、Excel 或 golden。
仍不允许 domain/rebar 直接依赖 OCCT / AIS。
```
