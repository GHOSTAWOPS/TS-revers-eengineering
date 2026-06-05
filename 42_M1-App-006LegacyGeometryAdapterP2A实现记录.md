# M1-App-006 LegacyGeometryAdapter P2A 实现记录

## 目标

本文件记录 M1-App-006 的实现边界和实际结果。

目标是：

```text
继续把 OCCT 几何能力包装成旧 VisualTS 钢筋逻辑可用的距离、最近点和曲线切向语义。
```

本阶段仍然不是钢筋生成。

## 为什么先做 P2A

旧图石线配筋、弧形筋和查询检查链路里会反复出现：

```text
选边 / 选实体
  -> 计算实体距离
  -> 取曲线参数点
  -> 判断曲线方向
  -> 后续按旧规则排布或检查钢筋
```

因此 P2A 先补：

- edge 中点切向。
- 任意两个 selection ref 的距离。
- 最近点对。

这比直接进入扫掠、剖切、偏移更稳，因为距离和切向是后续复杂能力的共同底座。

## 新增能力

### Edge differential

新增 DTO：

```text
LegacyEdgeDifferential
```

字段：

- `stableId`
- `parameter`
- `point`
- `tangent`

新增查询：

```text
edgeDifferentialAtParameter(ref, parameter)
```

实现口径：

- 使用 `BRepAdaptor_Curve::D1(parameter, point, tangent)`。
- tangent 输出前归一化。
- 参数越界返回稳定诊断。
- OCCT 类型不出 adapter。

### Distance / closest points

新增 DTO：

```text
LegacyDistanceMeasure
```

字段：

- `firstStableId`
- `secondStableId`
- `distance`
- `pointOnFirst`
- `pointOnSecond`
- `pointsValid`

新增查询：

```text
distanceBetween(firstRef, secondRef)
```

实现口径：

- 使用 `OccSelectionIndex` 解析两个 `LegacySelectionRef`。
- 使用 `BRepExtrema_DistShapeShape` 计算距离。
- 输出第一个最近点解。
- missing part / out-of-range 等错误继续沿用 selection resolve 诊断。

## 修改文件

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 未做事项

以下不进入 P2A：

- 线面求交。
- 边裁剪。
- 曲线 split。
- 曲线 offset。
- 扫掠。
- 剖切。
- 布尔。
- 钢筋生成。

这些进入 P2B/P2C，必须按旧 VisualTS 具体命令需要继续补。

## 验证结果

窄测试：

```text
legacy_geometry_adapter_tests.exe ..\..\123.stp
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 159.54 sec
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
M1-App-006 = pass
```

正式 app 已经具备：

```text
STEP 导入
AIS 显示
selection-v1 稳定选择引用
EDGE / FACE P0/P1 几何摘要
EDGE tangent / differential
selection ref 距离和最近点对
```

下一步建议：

```text
M1-App-007：LegacyGeometryAdapter P2B 已完成。

详见：
  43_M1-App-007LegacyGeometryAdapterP2B实现记录.md
```
