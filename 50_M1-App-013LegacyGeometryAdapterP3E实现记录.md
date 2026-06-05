# M1-App-013 LegacyGeometryAdapter P3E 实现记录

## 结论

本轮完成 `TODO-013 / M1-App-013`。

新增 `LegacyGeometryAdapter P3E`：

```text
buildSplineFromPoints
```

它对齐旧 VisualTS / ACIS 线索：

```text
sub_1405D5670
  -> 采样点列
  -> 点数按 max(5, length * 50) 一类规则保护
  -> api_curve_spline(pointCount, points, ..., &edge, ...)
```

本轮实现的是点列到 spline 的 legacy summary，不是旧 ACIS `EDGE`
topology mutation，也不是钢筋创建业务。

## 新增能力

新增 DTO：

```text
LegacySplineCurveBuild
```

字段：

```text
inputPoints
inputPointCount
requestedSampleCount
effectiveSampleCount
legacySuggestedSampleCount
sourcePolylineLength
length
curveKind
bounds
samplePoints
buildable
failureReason
```

新增 adapter API：

```text
buildSplineFromPoints(points, sampleCount)
```

当前语义：

```text
std::vector<LegacyPoint3d> -> OCCT GeomAPI_PointsToBSpline -> LegacySplineCurveBuild
```

`legacySuggestedSampleCount` 记录旧证据里的：

```text
max(5, sourcePolylineLength * 50)
```

`effectiveSampleCount` 当前使用调用方传入的 `sampleCount` 并保护最小 5。
这是为了避免 summary 阶段因为真实工程单位差异一次性生成巨大点列。

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- 从已有 edge 采样 5 个点并成功生成 spline summary。
- 生成曲线标记为 `LegacyCurveKind::BSpline`。
- 输出 `samplePoints`、`length`、`bounds`。
- `sampleCount=1` 时保护为最小 5。
- 点数不足时稳定拒绝，并把失败原因写入 DTO。
- 重复 / 折叠点列导致长度过短时稳定拒绝，并把失败原因写入 DTO。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: buildSplineFromPoints 不是 OccLegacyGeometryAdapter 的成员
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
Total Test time = 171.85 sec
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
sub_1405D5670
api_curve_spline
```

结果：

```text
No database bound for this context.
Database/session not found: ...\VisualTS.exe.i64
```

所以本轮没有新增 IDA 闭环证据。旧逻辑依据仍来自既有文档：

```text
15_线配筋与弧形组专项初稿.md
16_seg_steelbargroup字段地图初稿.md
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

本轮没有进入 `TODO-014`。

`buildSplineFromPoints` 是 spline rebuild summary，不等价于旧 ACIS
`api_curve_spline` 返回并写回实体的完整 topology mutation。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段有两个选择：

```text
TODO-014：LegacyWireChain 边链 / wire chain 语义
```

或：

```text
TODO-020：补 IDA MCP 旧线筋/弧筋链证据
```

如果继续补齐几何兼容层，建议做 `TODO-014`。如果准备靠近
线筋 / 弧筋业务算法层，建议先补 `TODO-020`。
