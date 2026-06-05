# M1-App-004 LegacyGeometryAdapter P0 实现记录

## 目标

本文件记录 M1-App-004 的实现边界和实际结果。

目标是：

```text
把 OCCT 的 EDGE / FACE 查询能力包装成旧 VisualTS 钢筋逻辑可调用的 legacy 几何语义。
```

本阶段仍然不是钢筋生成。

## 架构边界

正确边界：

```text
LegacySelectionRef
  -> OccSelectionIndex resolve
  -> TopoDS_Edge / TopoDS_Face
  -> OCCT 几何查询
  -> LegacyEdgeGeometry / LegacyFaceGeometry
```

钢筋业务层后续只能看到：

```text
LegacyEdgeGeometry
LegacyFaceGeometry
LegacyCurveKind
LegacySurfaceKind
```

不能看到：

```text
TopoDS_Edge
TopoDS_Face
BRepAdaptor_Curve
BRepAdaptor_Surface
BRepGProp
```

## 父目录复用边界

本轮复用父目录的是：

- `GeometryQueryService.h` 的几何查询模式。
- `BRepAdaptor_Curve` 查询曲线类型、端点、参数区间、闭合状态。
- `BRepGProp::LinearProperties` 查询边长。
- `BRepAdaptor_Surface` 查询面类型。
- `BRepGProp::SurfaceProperties` 查询面积。
- `surface.D1(u, v)` + `du.Crossed(dv)` 查询面法向，并处理 `TopAbs_REVERSED`。
- `TopExp::MapShapes(face, TopAbs_EDGE, ...)` 查询边界 edge 数。

本轮没有迁移：

```text
src/rebar/*
src/core/RebarCreationCommandService.*
EdgeToRebarFactory
FaceRebarGenerator
PolylineRebarGenerator
RebarFaceOffsetService
RebarLocalMoveService
```

原因：这些是父目录“OCCT 直接重写钢筋”的业务链，不能作为旧 VisualTS 1:1 复刻真相。

## 新增模块

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
```

新增测试：

```text
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

新增 CMake target：

```text
tsrebar_occ_legacy_adapter
legacy_geometry_adapter_tests
legacy_geometry_adapter_123
```

## P0 能力

### Edge

当前 `edgeGeometry()` 输出：

- `stableId`
- `curveKind`
- `length`
- `firstParameter`
- `lastParameter`
- `startPoint`
- `endPoint`
- `closed`

### Face

当前 `faceGeometry()` 输出：

- `stableId`
- `surfaceKind`
- `area`
- `samplePoint`
- `normal`
- `boundaryEdgeCount`

## 当前未做

以下不进入 P0，避免 adapter 过早膨胀：

- 点到实体距离。
- 最近点。
- 曲线采样点数组。
- bbox。
- 圆/椭圆/圆柱/圆锥/球/环面参数字段。
- 线面求交。
- 边裁剪。
- 偏移。
- 扫掠。
- 剖切。
- 钢筋生成。

这些进入后续 P1/P2。

## 验证结果

构建：

```text
success
```

CTest：

```text
8 / 8 passed
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

`legacy_geometry_adapter_123` 验证：

- `123.stp` 可导入。
- 第一个 selectable edge 可查询非零长度。
- edge 起终点不塌缩。
- edge curve kind 可分类。
- 第一个 selectable face 可查询非零面积。
- face boundary edge count 可查询。
- face surface kind 可分类。
- face normal 可得到单位向量。
- stableId 在查询结果中保留。

Readiness gate：

```text
M1-Formal-Ready
78 / 78 pass
0 error
```

## 当前结论

```text
M1-App-004 = pass
```

正式 app 已经具备：

```text
STEP 导入
AIS 显示
selection-v1 稳定选择引用
EDGE / FACE P0 几何摘要查询
```

下一步：

```text
M1-App-005：LegacyGeometryAdapter P1 已完成。

详见：
  41_M1-App-005LegacyGeometryAdapterP1实现记录.md
```
