# M1-App-021 AIS 钢筋显示映射实现记录

## 结论

本轮完成 `TODO-022 / M1-App-021`。

新增 presentation 层 adapter：

```text
RebarAisPresentationAdapter
RebarAisPresentationResult
RebarAisDisplayItem
```

本轮把 `TODO-021` 已能生成的 domain `SteelData -> SteelBarGroup ->
SteelBar -> SteelBarSegment` 映射为 OCCT AIS 可显示对象：

```text
domain SteelBarGroup / SteelBarSegment
  -> presentation/occ/RebarAisPresentationAdapter
  -> TopoDS_Edge
  -> AIS_Shape
```

本轮不是钢筋创建业务，不接 Detail writer，不接新工程格式 runtime，
也不声明旧 HOOPS 显示效果已经完全一致。

## 新增能力

### line segment 显示映射

- `SteelBarSegmentShape::Line` 使用 `startPoint / endPoint` 创建
  `TopoDS_Edge`。
- 返回 `AIS_Shape`，供 OCCT AIS viewer 显示。
- 测试确认生成的 edge 为 OCCT line curve。

### arc segment 显示映射

- `SteelBarSegmentShape::Arc` 使用 `startPoint / middlePoint / endPoint`
  创建圆弧 edge。
- 返回 `AIS_Shape`。
- 测试确认生成的 edge 为 OCCT circle curve。

### 稳定诊断

已覆盖：

- missing group。
- empty group / no displayable segments。
- missing / hidden bar 作为不可显示原因进入诊断。
- unsupported segment shape 稳定拒绝。

## 边界

本轮允许 AIS / OCCT 出现在：

```text
app/src/presentation/occ
app/tests/unit/rebar_ais_presentation_tests.cpp
```

本轮没有让 `domain/rebar` 依赖 AIS / OCCT。

本轮没有迁入父目录钢筋业务代码：

```text
RebarCreationCommandService
EdgeToRebarFactory
FaceRebarGenerator
PolylineRebarGenerator
src/rebar/*
```

本轮没有用 OCCT 定义钢筋业务真相。OCCT 只负责把已经存在的 domain
segment 点位映射成显示 shape。

## IDA / 旧图石确认判断

本轮不新增 IDA MCP 查询。

原因：

```text
TODO-022 是 presentation 层显示映射节点。
输入已经是 TODO-021 生成的 domain SteelBarGroup / SteelBarSegment。
本轮只把既有点位转换成 AIS_Shape，不决定钢筋生成、编辑、统计、出图规则。
```

本轮可引用的旧逻辑证据：

```text
E-IDA-022
  -> 支撑 TODO-021 线筋 / 弧筋创建 spike 的旧 VisualTS 调用链。

E-DEV-045
  -> 支撑本轮 AIS 显示映射实现和验证。
```

未闭合边界：

```text
旧 HOOPS 显示样式、选择高亮、颜色、线宽、交互反馈是否 1:1，
仍需后续旧图石运行确认或 IDA / UI 证据补充。
```

## TDD

RED：

```text
cmake --build app\build --target rebar_ais_presentation_tests
result = expected compile failure
failure = presentation/occ/RebarAisPresentationAdapter.h missing
```

GREEN：

```text
cmake --build app\build --target rebar_ais_presentation_tests
app\build\rebar_ais_presentation_tests.exe
result = pass
```

新增测试：

```text
rebar_ais_presentation_tests
```

覆盖：

- line segment -> AIS / OCCT edge。
- arc segment -> AIS / OCCT circular edge。
- empty group 稳定诊断。
- missing group 稳定诊断。

## 验证结果

默认 CTest：

```text
11 / 11 passed
Total Test time = 124.85 sec
```

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

`domain/rebar` 边界扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app\src\domain\rebar
result = no matches
```

`git diff --check`：

```text
pass
```

## xhigh 只读 review

结论：

```text
Critical = none
Important = none
Minor =
  RebarAisPresentationAdapter.cpp 使用 std::move，建议显式补 <utility>。
Route Drift = none
Verdict = allow_commit
```

处理：

```text
已补 <utility>。
xhigh 子代理已关闭。
```

## 修改范围

```text
app/CMakeLists.txt
app/src/presentation/occ/RebarAisPresentationAdapter.h
app/src/presentation/occ/RebarAisPresentationAdapter.cpp
app/tests/unit/rebar_ais_presentation_tests.cpp
```

## 未完成项

本轮不声明以下事项完成：

- 旧 HOOPS 显示样式 1:1。
- 线筋 / 弧筋 UI handler 已接入真实创建流程。
- Viewer 中通过按钮实时创建并显示钢筋。
- Detail writer 从 domain rebar 输出工程图包。
- 新工程格式保存 / 读取这些钢筋对象。
- 旧图石 golden 对照。

## 下一步建议

下一阶段建议做：

```text
TODO-023 / M1-App-022
新设计文件格式 runtime P1
```

原因：

```text
domain 钢筋对象已经能创建，presentation/occ 已能映射为 AIS 显示对象。
下一步需要把 STEP 来源、selection-v1 refs、rebar groups、binding 和 evidence
接入正式 app 的新工程保存 / 读取链路。
```

如果要先补证据，也可以先做旧图石运行确认：

```text
线配筋 / 扇形筋 / 同心圆真实 UI 流程
输出钢筋 STP 复杂样本
```
