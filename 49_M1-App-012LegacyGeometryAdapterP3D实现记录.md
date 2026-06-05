# M1-App-012 LegacyGeometryAdapter P3D 实现记录

## 结论

本轮完成 `TODO-012 / M1-App-012`。

新增 `LegacyGeometryAdapter P3D`：

```text
pointToEdgeGroupDistance
```

它对齐旧 VisualTS / ACIS 线索：

```text
sub_14059B980
  -> 遍历组内已有 edge
  -> 对每个 edge 调 api_entity_point_distance
  -> 返回输入点到组内 edge 的最小距离
```

本轮实现的是点到 edge ref 列表的 legacy distance summary，不是钢筋组业务对象、
不是旧链表结构复刻，也不是几何 topology mutation。

## 新增能力

新增 DTO：

```text
LegacyEdgeGroupDistanceCandidate
LegacyEdgeGroupDistance
```

字段：

```text
inputPoint
threshold
minDistance
tooClose
hasNearest
nearest.edgeStableId
nearest.distance
nearest.pointOnEdge
nearest.edgeParameter
nearest.edgeParameterValid
candidates
```

新增 adapter API：

```text
pointToEdgeGroupDistance(point, edgeRefs, threshold)
```

当前候选组表达方式：

```text
std::vector<LegacySelectionRef>
```

这符合本阶段边界：adapter 只看旧几何引用，不引入钢筋业务对象。

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- 单 edge candidate：点在 edge 上时最小距离接近 0。
- 多 edge candidates：返回最小距离所属 edge stableId。
- threshold 命中：距离 `<= 0.002` 时 `tooClose=true`。
- outside point：距离大于阈值时 `tooClose=false`。
- 空 group：稳定拒绝。
- wrong type / missing ref：返回稳定 diagnostic。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: pointToEdgeGroupDistance 不是 OccLegacyGeometryAdapter 的成员
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
Total Test time = 131.42 sec
```

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

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

本轮没有进入 `TODO-013`。

`pointToEdgeGroupDistance` 是点到候选 edge refs 的 min-distance summary，
不等价于旧 `seg_steelbargroup` 链表对象完整复刻。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段有两个选择：

```text
TODO-013：LegacyGeometryAdapter P3E 点列 spline 重建
```

或：

```text
TODO-020：补 IDA MCP 旧线筋/弧筋链证据
```

如果继续沿 `sub_1405D5670` 的几何能力补齐，建议做 `TODO-013`。
如果准备靠近业务算法层，建议先补 `TODO-020`。
