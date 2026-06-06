# M1-App-018 Rebar Domain Model Freeze P1 实现记录

## 结论

本轮完成 `TODO-018 / M1-App-018`。

新增 / 补强 `domain/rebar` P1 领域模型：

```text
SteelData
SteelBarGroup
SteelBar
SteelBarSegment
RebarDomainTypes
```

本轮目标是把旧 VisualTS / Detail / SFL 证据沉到可编码的领域对象边界：

```text
legacyObject.raw
geometryRef
binding
evidence
unresolved legacy fields
Detail key fields
group -> bar -> segment ID references
```

本轮没有实现线筋、弧筋、面筋或任何钢筋创建算法。

## 新增能力

新增纯领域类型：

```text
DomainPoint3d
RebarEvidenceRef
LegacyRawField
LegacyRawBlock
UnresolvedLegacyField
GeometryReference
BindingState
BindingItem
BindingAnchor
SteelBarSegmentShape
detailShapeTypeCode
```

补强领域对象字段：

```text
SteelData
  -> steelDataId / level / gradeName / diameterSet
  -> legacyRaw / binding / unresolvedLegacyFields / evidence
  -> segments / bars / groups

SteelBarGroup
  -> groupId / rsdId / displayNumber / actualNumber
  -> componentName / projectSteelName / createCommand / steelDataId
  -> diameter / secondaryDiameter / interval / barCount / segmentCount
  -> steelLevel / layer / profile / use / rangeLess180
  -> steelWay / rebarType / offsetInOut
  -> sourceFaceId / sourceLineId / sourceCurveIds / referenceIds / barIds
  -> createdFromParameters / legacyRaw / geometryRef / binding / evidence

SteelBar
  -> barId / groupId / sequenceNo / displayNumber
  -> diameter / steelLevel / segmentIds / length / shapeType
  -> active / visible / legacyRaw / geometryRef / binding / evidence

SteelBarSegment
  -> segmentId / barId / sequenceNo / shapeType
  -> startPoint / endPoint / middlePoint
  -> startRadius / endRadius / offset / length
  -> legacyRaw / geometryRef / binding / unresolvedLegacyFields / evidence
```

其中 `id` / `legacyCommand` / `segmentType` / `lengthMm` 旧字段暂时保留为过渡 alias，
避免破坏既有薄骨架；后续消费者应以 `groupId / barId / segmentId` 等 P1 字段为业务真相。

## 证据对齐

字段来源：

```text
09_钢筋领域模型草案.md
13_Detail字段映射矩阵.md
16_seg_steelbargroup字段地图初稿.md
E-IDA-018
E-IDA-019
```

关键对齐点：

- `rsdId` 保留 string，支持 `Y%d / Z%d / %d%c` 等非纯 int 形式。
- `StbGroup` 关键字段进入 `SteelBarGroup`。
- `StbGeo` 关键字段进入 `SteelBarSegment`。
- `StbTable / StbRow` 需要的 `rsdId / diameter / steelLevel / length / segmentCount` 能从同一份 `RebarModel` 派生。
- `legacyObject.raw + geometryRef + binding + evidence` 作为新格式和旧证据之间的连接层。
- `unresolvedLegacyFields` 显式承载低置信字段，不把未知旧字段写成确定事实。

## TDD

RED：

```text
cmake --build app\build --target rebar_domain_model_tests
result = expected compile failure
failure = SteelData.steelDataId / BindingState / SteelBarGroup.rsdId /
          SteelBarSegment.shapeType / legacyRaw / geometryRef / binding 等字段缺失
```

GREEN 窄测试：

```text
cmd.exe /c 'call "D:\Visual Studio 2026\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build app\build --target rebar_domain_model_tests && app\build\rebar_domain_model_tests.exe'
result = pass
```

## 测试结果

默认 CTest：

```text
9 / 9 passed
Total Test time = 163.30 sec
```

新增测试：

```text
rebar_domain_model_tests
```

覆盖：

- 默认 binding / active / visible / unknown shape 状态。
- group -> bar -> segment ID 引用关系。
- Detail 关键字段：`rsdId / diameter / interval / barCount / segmentCount / steelWay / rebarType`。
- `SteelBarSegmentShape::Line -> Detail shapeType L`。
- low-confidence legacy field、evidence、binding item 显式承载。

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

`domain/rebar` 边界扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
result = no matches
```

xhigh 只读 review：

```text
Critical = none
Important = none
Minor =
  - SteelBar / SteelBarGroup / SteelBarSegment 保留旧 id alias；
    后续消费者应明确只以新 ID 字段为业务真相。
  - 测试覆盖了段点位 / offset / length，但未显式赋值 startRadius / endRadius；
    字段已存在，不阻塞本轮 commit。
Route Drift = none
Verdict = allow_commit
```

xhigh 子代理已在 review 结束后关闭。

## 修改范围

```text
app/CMakeLists.txt
app/src/domain/rebar/RebarDomainTypes.h
app/src/domain/rebar/SteelData.h
app/src/domain/rebar/SteelBar.h
app/src/domain/rebar/SteelBarSegment.h
app/src/domain/rebar/SteelBarGroup.h
app/tests/unit/rebar_domain_model_tests.cpp
```

## 边界

本轮没有实现钢筋生成。

本轮没有迁移父目录钢筋业务主链。

本轮没有实现 Detail writer、Save/Open runtime、UI 命令 handler 或 AIS 钢筋显示。

`domain/rebar` 仍不依赖 OCCT / AIS。

`RebarDomainTypes` 是 P1 可编码字段冻结，不等于旧图石所有业务字段语义已闭合。

## 下一步建议

下一阶段建议优先做：

```text
TODO-019：旧命令契约绑定 P1
```

原因：

```text
领域模型已具备承载钢筋对象的 P1 边界。下一步可以把线筋 / 弧筋 / 裁剪等
首批旧命令接入 LegacyUiCommandMap 和 CommandRegistry，用 NotImplemented
或真实 handler 占位把 UI 命令、输入选择类型和后续业务算法入口对齐。
```

如果 IDA 数据库恢复绑定，也可以先补：

```text
TODO-020：IDA MCP 旧线筋 / 弧筋链证据。
```
