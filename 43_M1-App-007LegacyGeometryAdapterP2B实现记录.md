# M1-App-007 LegacyGeometryAdapter P2B 实现记录

## 目标

本文件记录 M1-App-007 的实现边界和实际结果。

目标是：

```text
继续把 OCCT 几何能力包装成旧 VisualTS 钢筋逻辑可用的 edge-face 接触 / 相交语义。
```

本阶段仍然不是钢筋生成。

## 为什么做 P2B

旧图石线配筋、面配筋、剖切和裁剪类命令都需要判断：

```text
边和面有没有关系？
接触点在哪里？
接触点在边的哪个参数位置？
```

因此 P2B 先补一个保守接口：

```text
edgeFaceIntersections(edgeRef, faceRef)
```

它不把 OCCT section / extrema 类型暴露给业务层。

## 新增能力

### Boundary edge stable ids

`LegacyBoundaryLoop` 新增：

```text
edgeStableIds
```

用途：

- face 查询结果能暴露边界 edge 的稳定引用。
- 后续命令可以从 face boundary loop 继续进入 edge 级几何查询。
- 测试可以稳定选择“属于当前 face 的 edge”，避免拿随机 edge 和随机 face 碰运气。

### Edge-face intersections

新增 DTO：

```text
LegacyIntersectionPoint
LegacyIntersectionSet
```

新增查询：

```text
edgeFaceIntersections(edgeRef, faceRef)
```

输出字段：

- `edgeStableId`
- `faceStableId`
- `points`
- `overlap`

每个 point 包含：

- `point`
- `edgeParameter`
- `edgeParameterValid`

实现口径：

- 使用 `OccSelectionIndex` 解析 edge / face。
- 使用 `BRepExtrema_DistShapeShape` 判定接触/重叠。
- 当距离小于容差时输出代表交点。
- 优先使用 `ParOnEdgeS1` 获取 edge 参数。
- 若参数不可得，则用 `Extrema_ExtPC` 投影到 edge 上补参数。

## 重要边界

P2B 是 edge-face 接触 / 代表点查询，不是完整布尔 section。

当前不声明：

- 线面完整交线求解。
- edge 按 face 精确裁剪。
- face 被 edge 分割。
- 扫掠。
- 剖切。
- 布尔。
- 钢筋生成。

这些进入 P2C/P3 或具体命令专项。

## 修改文件

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 验证结果

窄测试：

```text
legacy_geometry_adapter_tests.exe ..\..\123.stp
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 147.45 sec
```

通过项：

- `command_registry_tests`
- `legacy_selection_ref_tests`
- `app_smoke`
- `viewer_smoke_tests`
- `domain_rebar_boundary`
- `step_import_probe_123`
- `occ_selection_index_123`
- `legacy_geometry_adapter_123`

Readiness gate：

```text
M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

## 当前结论

```text
M1-App-007 = pass
```

正式 app 已经具备：

```text
STEP 导入
AIS 显示
selection-v1 稳定选择引用
EDGE / FACE P0/P1 几何摘要
EDGE tangent / differential
selection ref 距离和最近点对
face boundary edge stable ids
edge-face 接触/重叠代表点
```

下一步建议：

```text
M1-App-008：LegacyGeometryAdapter P2C 已完成。

详见：
  44_M1-App-008LegacyGeometryAdapterP2C实现记录.md
```
