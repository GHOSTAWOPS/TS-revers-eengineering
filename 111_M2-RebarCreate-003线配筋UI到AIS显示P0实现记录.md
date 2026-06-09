# M2-RebarCreate-003 线配筋 UI 到 AIS 显示 P0 实现记录

todoId = TODO-073
phase = M2-RebarCreate-003
evidenceIds = E-DEV-095

## 目标

本轮只完成 `TODO-073 / 线配筋 UI 到 AIS 显示 P0：命令完成后显示创建的 SteelBarGroup`。

目标是把 `Rebar.Create.LineGroup` 成功创建的 domain `SteelBarGroup` 接到 UI 可见反馈 / OCCT AIS 显示路径。

本轮不是完整线配筋算法，也不是用 OCCT 直接重写钢筋。

## 已实现

新增 viewer 显示入口：

```text
RebarAisPresentationResult
  -> OccViewerWidget::displayRebarPresentation(...)
  -> AIS_InteractiveContext::Display(...)
  -> displayedRebarShapeCount
```

`MainWindow` 在命令成功后接入显示链：

```text
CommandId::RebarLineCreate
  -> 刷新本次 P0 group/bar/segment id
  -> RebarLineGroupCommandHandler
  -> SteelData append
  -> RebarAisPresentationAdapter::buildGroupPresentation(...)
  -> OccViewerWidget::displayRebarPresentation(...)
```

连续创建时不复用固定 group id：

```text
ui-line-group-p0-1
ui-line-group-p0-2
...
```

说明：这是 TODO-073 为避免重复显示旧组而引入的 P0 临时 id，不等价旧图石最终对象编号规则。

失败路径保持不刷新：

```text
empty selection / wrong type / geometry failure / normalizer failure
  -> CommandStatus::Failed
  -> 不调用显示刷新
  -> displayedRebarShapeCount 不变
```

## 测试覆盖

新增 `line_group_display_smoke_tests`：

```text
MainWindow
  -> displayDocument(123.stp)
  -> 未选择对象时触发 Rebar.Create.LineGroup
  -> displayedRebarShapeCount 不变
  -> 选择真实 edge stableId
  -> 触发 Rebar.Create.LineGroup
  -> displayedRebarShapeCount 增加
  -> 再触发一次 Rebar.Create.LineGroup
  -> lastDisplayedRebarGroupId 变化，证明显示的是第二个新组
```

该测试使用 `123.stp` 真实旧图石钢筋 STP witness。

测试需要真实 Windows Qt platform 承载 OCCT native window。为避免打扰桌面，测试窗口设置：

```text
Qt::WA_DontShowOnScreen
```

## 修改文件

```text
app/CMakeLists.txt
app/src/app/MainWindow.cpp
app/src/app/MainWindow.h
app/src/command/RebarLineGroupCommandHandler.cpp
app/src/command/RebarLineGroupCommandHandler.h
app/src/presentation/occ/OccViewerWidget.cpp
app/src/presentation/occ/OccViewerWidget.h
app/tests/integration/line_group_display_smoke_tests.cpp
app/tests/unit/rebar_line_group_command_handler_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

## TDD

红灯：

```text
line_group_display_smoke_tests 初始构建失败：
  OccViewerWidget 没有 displayedRebarShapeCount()

xhigh Important 修复阶段新增红灯：
  rebar_line_group_command_handler_tests 链接失败：
  RebarLineGroupCommandHandler::setParameters 未实现
```

绿灯：

```text
line_group_display_smoke_123 = pass
rebar_line_group_command_handler_tests = pass
```

中间诊断：

```text
QT_QPA_PLATFORM=minimal 下 OCCT native viewer 退出不稳定。
改为 windows platform + WA_DontShowOnScreen 后稳定通过。
```

## 验证

```text
targetedBuild = pass; line_group_display_smoke_tests
targetedTest = pass; line_group_display_smoke_123
CTest = pass; 20/20 pass
readinessGateUnit = pass; 33/33 pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
domainRebarBoundary = pass
domainRebarCommandOCCLeak = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches in app/src/domain/rebar or app/src/command
parentRebarBusinessReference = pass; no forbidden parent rebar factory references in app code
gitDiffCheck = pass
xhighReview = needs_fix; critical review placeholder fixed; important duplicate group display fixed; route drift none
```

## xhigh Review 处理

只读代理 `Socrates` 结论：

```text
Verdict = needs_fix
Critical = 文档 / report / JSON 中仍有 review 占位状态，导致 readiness gate 失败
Important = 连续两次创建时固定 groupId 可能让显示链重复显示旧 group
Route Drift = 无
```

主流程修复：

```text
1. 文档、run report、JSON 和 46 改成真实 review 结论。
2. MainWindow 每次执行 Rebar.Create.LineGroup 前生成新的 P0 group/bar/segment id。
3. RebarLineGroupCommandHandler 增加 setParameters(...)，供 UI 参数链刷新。
4. OccViewerWidget 记录 lastDisplayedRebarGroupId。
5. smoke 测试增加连续两次成功创建，断言第二次显示的是新 group。
```

## 边界

本轮允许 AIS / OCCT 出现在：

```text
app/src/presentation/occ
app/src/app/MainWindow.cpp
app/tests/integration/line_group_display_smoke_tests.cpp
```

说明：

- `presentation/occ` 是 OCCT AIS 显示层。
- `MainWindow` 只做 UI 编排，使用 `RebarAisPresentationAdapter`，不定义钢筋业务真相。
- `domain/rebar` 和 `command/RebarLineGroupCommandHandler` 仍不直接依赖 `TopoDS_ / AIS_ / BRep / TopAbs_`。

本轮没有迁入父目录钢筋业务代码：

```text
RebarCreationCommandService
EdgeToRebarFactory
FaceRebarGenerator
PolylineRebarGenerator
src/rebar/*
```

## 明确不证明

```text
不证明完整旧线配筋算法。
不证明旧 VisualTS 完整 ENTITY_LIST 选择对象语义。
不证明旧 UI 参数窗口、状态栏提示、失败提示已经 1:1。
不证明 undo / dirty / 保存打开 / golden。
不证明面配筋、弧筋、接头、Excel 或 AutoCAD L2。
不证明旧 HOOPS 显示样式、颜色、线宽和高亮效果已经 1:1。
```

## 下一步

建议下一步进入：

```text
TODO-074 / M2-RebarCreate-004
线配筋参数窗口 P0：旧 UI 字段占位、默认值和 handler 参数接入
```

边界：

```text
继续只处理 Rebar.Create.LineGroup。
不扩展到面配筋、弧筋、接头、Excel 或 golden。
参数默认值不确定时必须记录 gap，不写成旧逻辑已确认。
```
