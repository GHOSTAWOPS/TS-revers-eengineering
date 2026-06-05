# M1-App-009 LegacyGeometryAdapter P3A 实现记录

## 目标

本文件记录 M1-App-009 的实现边界和实际结果。

目标是：

```text
把 edge split 包装成旧 VisualTS 钢筋逻辑可用的曲线分割语义。
```

本阶段仍然不是钢筋生成。

## 为什么做 P3A

现有 IDA 文档证据已经确认：

```text
sub_1405D5670
  -> api_split_curve
  -> api_curve_spline
  -> 端部裁剪
  -> 组内最小距离判断
```

其中 `api_split_curve` 是 `seg_steelbargroup` 曲线修正链的核心步骤。

所以 P3A 先补：

```text
edgeSplitAtParameter(edgeRef, splitParameter, sampleCountPerInterval)
```

它把一条 edge 按参数拆成两个 `LegacyCurveInterval`。

## 新增能力

新增 DTO：

```text
LegacyCurveSplit
```

字段：

- `sourceEdgeStableId`
- `splitParameter`
- `splitPoint`
- `firstInterval`
- `secondInterval`

新增查询：

```text
edgeSplitAtParameter(edgeRef, splitParameter, sampleCountPerInterval)
```

实现口径：

- 使用 `BRepAdaptor_Curve` 读取 edge 参数范围。
- split 参数必须落在 edge 内部，不能贴近端点。
- 端点 guard 使用 `max((last-first)*0.001, Precision::Confusion())`。
- 内部复用 `edgeInterval()` 输出前后两个子段。
- 不创建新的 `TopoDS_Edge`。
- 不修改模型拓扑。

## 重要边界

P3A 是 split 语义摘要，不是完整 topology split。

当前不声明：

- 新建真实 trimmed edge。
- 修改模型拓扑。
- spline 重建。
- 端部迭代裁剪。
- curve offset。
- 扫掠。
- 剖切。
- 钢筋生成。

这些进入后续 P3B/P3C 或线配筋专项。

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
Total Test time = 150.34 sec
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
M1-App-009 = pass
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
edge 参数区间 / 子段长度 / 子段采样
edge split 语义 / 前后子段
```

下一步建议：

```text
M1-App-010：LegacyGeometryAdapter P3B。

优先补：
  点到 edge 的投影参数查询
  支持旧 api_split_curve(edge, distancePoint, ...) 的输入形式
```

