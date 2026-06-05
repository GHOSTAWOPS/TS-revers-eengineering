# M1-App-005 LegacyGeometryAdapter P1 实现记录

## 目标

本文件记录 M1-App-005 的实现边界和实际结果。

目标是：

```text
继续把 OCCT 几何能力包装成旧 VisualTS 钢筋逻辑可用的 EDGE / FACE 语义。
```

本阶段仍然不是钢筋生成。

## 架构边界

正确边界保持不变：

```text
LegacySelectionRef
  -> OccSelectionIndex resolve
  -> TopoDS_Edge / TopoDS_Face
  -> OCCT 几何查询
  -> LegacyEdgeGeometry / LegacyFaceGeometry
```

钢筋业务层后续只能看到：

```text
LegacyBoundingBox
LegacyBoundaryLoop
LegacyEdgeGeometry
LegacyFaceGeometry
LegacyPoint3d
LegacyVector3d
```

不能看到：

```text
TopoDS_Edge
TopoDS_Face
BRepBndLib
BRepAdaptor_Curve
BRepTools::OuterWire
TopExp_Explorer
```

## 新增能力

### Edge

`edgeGeometry()` 在 P0 基础上新增：

- `bounds`
- `fingerprint`

新增查询：

- `edgePointAtParameter(ref, parameter)`
- `edgeSamplePoints(ref, sampleCount)`

实现口径：

- bbox 由 `BRepBndLib::AddOptimal` 计算。
- 参数点由 `BRepAdaptor_Curve::Value(parameter)` 计算。
- sample 点按曲线参数区间均匀采样，首尾点对齐 edge 起终点。
- fingerprint 使用 `edge-fingerprint-v1` 前缀，纳入 stableId、曲线类型、长度、参数区间、bbox、闭合状态。

### Face

`faceGeometry()` 在 P0 基础上新增：

- `bounds`
- `fingerprint`
- `boundaryLoops`

实现口径：

- bbox 由 `BRepBndLib::AddOptimal` 计算。
- boundary loops 通过 `TopExp_Explorer(face, TopAbs_WIRE)` 收集。
- outer wire 通过 `BRepTools::OuterWire(face)` 判定，并保证 outer loop 排在第一位。
- 每个 loop 输出 `ordinal / outer / edgeCount / perimeter / bounds`。
- fingerprint 使用 `face-fingerprint-v1` 前缀，纳入 stableId、曲面类型、面积、边界 edge 数、loop 数、bbox。

### 诊断

本轮补齐的诊断场景：

- `edgeGeometry(faceRef)` 返回稳定 wrong type 诊断。
- missing part ref 返回 `not in current document`。
- out-of-range subshape ref 返回 `out of range`。
- 非法采样数量返回稳定错误。
- 参数越界返回稳定错误。

## 修改文件

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 未做事项

以下不进入 P1：

- 点到边 / 点到面的最近距离。
- 两实体最小距离。
- 线面求交。
- 边裁剪。
- 曲线偏移。
- 扫掠。
- 剖切。
- 钢筋生成。

这些属于后续 P2/P3 几何能力，必须按具体旧 VisualTS 命令需要逐项补。

## 验证结果

窄测试：

```text
legacy_geometry_adapter_tests.exe ..\..\123.stp
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 149.30 sec
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
M1-App-005 = pass
```

正式 app 已经具备：

```text
STEP 导入
AIS 显示
selection-v1 稳定选择引用
EDGE / FACE P0 几何摘要查询
EDGE / FACE P1 bbox、采样、boundary loop、fingerprint 和诊断
```

下一步建议：

```text
M1-App-006：LegacyGeometryAdapter P2A 已完成。

详见：
  42_M1-App-006LegacyGeometryAdapterP2A实现记录.md
```
