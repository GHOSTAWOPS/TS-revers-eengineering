# M1-App-014 LegacyWireChain 实现记录

## 结论

本轮完成 `TODO-014 / M1-App-014`。

新增 `LegacyWireChain`：

```text
buildWireChain
```

它对齐旧 VisualTS / ACIS 语义边界：

```text
EDGE / ENTITY_LIST / face boundary loop
  -> 多段 edge ref
  -> 连通排序
  -> 总长度 / 起点 / 终点 / bbox
  -> closed / connected
```

本轮实现的是边链 legacy summary，不是钢筋排布业务，也不是 OCCT
wire topology mutation。

## 新增能力

新增 DTO：

```text
LegacyWireChainEdge
LegacyWireChain
```

字段：

```text
inputEdgeStableIds
orderedEdges
inputEdgeCount
orderedEdgeCount
totalLength
startPoint
endPoint
bounds
connected
closed
failureReason
```

新增 adapter API：

```text
buildWireChain(edgeRefs)
```

当前语义：

```text
std::vector<LegacySelectionRef> -> ordered edge chain summary
```

排序策略：

```text
从第一个 edge 开始，候选 edge 可接到链头或链尾。
若候选 edge 原方向和链方向相反，记录 reversed=true。
```

这允许 face boundary loop 或旧 ENTITY_LIST 一类输入在 adapter 层形成稳定的
legacy wire chain summary。

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- 从 face boundary loop 的 edge stableId 构造闭合 wire chain。
- 单 edge 输入返回 open wire chain，长度和端点与 edgeGeometry 一致。
- 非连通 edge refs 稳定拒绝，DTO 中携带 `connected=false` 和失败原因。
- wrong type / missing ref 返回稳定 diagnostic。
- 输出 `orderedEdges`、`totalLength`、`startPoint`、`endPoint`、`bounds`、`closed`。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: buildWireChain 不是 OccLegacyGeometryAdapter 的成员
```

GREEN 窄测试：

```text
cmake --build build --target legacy_geometry_adapter_tests
build\legacy_geometry_adapter_tests.exe ..\..\123.stp
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 126.51 sec
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
rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
result = no matches
```

## IDA 状态

本轮尝试用 IDA MCP 查询：

```text
ENTITY_LIST
sub_1405D5670
sgroupbarline
sgroupbararc
```

结果：

```text
No database bound for this context.
```

所以本轮没有新增 IDA 闭环证据。旧语义依据仍来自既有文档：

```text
15_线配筋与弧形组专项初稿.md
16_seg_steelbargroup字段地图初稿.md
35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md
```

该缺口已继续保留在 `99_缺口和待确认项.md`。

## 修改范围

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 边界

本轮没有实现钢筋生成。

本轮没有迁移父目录钢筋业务主链。

本轮没有进入 `TODO-015`。

`buildWireChain` 是 edge refs 到 wire chain 的 summary，不等价于 OCCT
`TopoDS_Wire` 生成，也不等价于旧图石 `ENTITY_LIST` 完整对象复刻。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段有两个选择：

```text
TODO-015：LegacyGeometryAdapter offset 曲线能力 spike
```

或：

```text
TODO-020：补 IDA MCP 旧线筋/弧筋链证据
```

如果继续补齐几何兼容层，建议做 `TODO-015`。如果准备靠近
线筋 / 弧筋业务算法层，建议先补 `TODO-020`。
