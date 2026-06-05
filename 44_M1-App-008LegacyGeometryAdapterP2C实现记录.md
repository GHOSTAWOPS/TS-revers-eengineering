# M1-App-008 LegacyGeometryAdapter P2C 实现记录

## 目标

本文件记录 M1-App-008 的实现边界和实际结果。

目标是：

```text
把 edge 参数区间包装成旧 VisualTS 钢筋逻辑可用的曲线子段语义。
```

本阶段仍然不是钢筋生成。

## 为什么做 P2C

旧图石线配筋、弧形筋和段组逻辑会围绕曲线段做：

```text
取 edge 参数区间
  -> 计算子段长度
  -> 取子段起止点
  -> 按子段采样 / 排布
  -> 后续再裁剪、偏移或拆分
```

因此 P2C 先补一个保守接口：

```text
edgeInterval(edgeRef, firstParameter, lastParameter, sampleCount)
```

它不创建新的 `TopoDS_Edge`，也不把 OCCT 类型暴露给业务层。

## 新增能力

新增 DTO：

```text
LegacyCurveInterval
```

字段：

- `sourceEdgeStableId`
- `firstParameter`
- `lastParameter`
- `length`
- `startPoint`
- `endPoint`
- `bounds`
- `samplePoints`

新增查询：

```text
edgeInterval(edgeRef, firstParameter, lastParameter, sampleCount)
```

实现口径：

- 使用 `BRepAdaptor_Curve` 读取 edge 参数范围。
- 使用 `GCPnts_AbscissaPoint::Length` 计算区间长度。
- 按参数区间均匀采样。
- 用采样点生成 interval bbox。
- 参数反向、越界、非有限值、采样数量非法时返回稳定诊断。

## 重要边界

P2C 是曲线区间摘要，不是几何拓扑裁剪。

当前不声明：

- 创建新的 trimmed edge。
- 修改模型拓扑。
- 曲线 offset。
- 扫掠。
- 剖切。
- 布尔。
- 钢筋生成。

这些进入 P3 或具体命令专项。

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
Total Test time = 156.36 sec
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
M1-App-008 = pass
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
```

下一步建议：

```text
M1-App-009：LegacyGeometryAdapter P3A 已完成。

详见：
  45_M1-App-009LegacyGeometryAdapterP3A实现记录.md
```
