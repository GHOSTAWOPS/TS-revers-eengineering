# M1-App-010 LegacyGeometryAdapter P3B 实现记录

## 结论

本轮完成 `TODO-010 / M1-App-010`。

新增 `LegacyGeometryAdapter P3B`：

```text
edgeProjectPoint
edgeSplitAtPoint
```

它对齐旧 VisualTS / ACIS 链路：

```text
api_entity_entity_distance
  -> 得到 edge 上的最近点 / 距离点
  -> api_split_curve 按这个点切 curve
```

## 新增能力

新增 DTO：

```text
LegacyEdgePointProjection
```

字段：

```text
edgeStableId
inputPoint
projectedPoint
parameter
distance
parameterValid
inside
```

新增 adapter API：

```text
edgeProjectPoint(edgeRef, point)
edgeSplitAtPoint(edgeRef, point, sampleCountPerInterval)
```

`edgeSplitAtPoint` 内部先调用 `edgeProjectPoint` 得到投影参数，再复用 `edgeSplitAtParameter` 输出 `LegacyCurveSplit`。

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- midpoint projection：距离接近 0，parameter valid，inside true。
- endpoint projection：parameter valid，但 inside false。
- midpoint split at point：返回前后两个 `LegacyCurveInterval`。
- endpoint split at point：稳定拒绝，诊断包含 `inside edge interval`。
- outside projection：投影参数可用，但 inside false，距离非 0。
- wrong type / missing part：稳定 diagnostic。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: edgeProjectPoint / edgeSplitAtPoint 不是 OccLegacyGeometryAdapter 的成员
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
Total Test time = 122.27 sec
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

本轮没有进入 `TODO-011`。

`edgeProjectPoint` / `edgeSplitAtPoint` 仍是 legacy geometry summary，不是真实 topology split。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段有两个选择：

```text
TODO-011：端点 trim 语义
```

或：

```text
TODO-020：补 IDA MCP 旧线筋/弧筋链证据
```

如果继续几何兼容层，建议先做 `TODO-011`。如果准备靠近钢筋业务层，建议先补 `TODO-020`。
