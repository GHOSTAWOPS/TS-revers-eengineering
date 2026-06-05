# M1-App-011 LegacyGeometryAdapter P3C 实现记录

## 结论

本轮完成 `TODO-011 / M1-App-011`。

新增 `LegacyGeometryAdapter P3C`：

```text
edgeTrimEndpoint
```

它对齐旧 VisualTS / ACIS 线索：

```text
sub_140580950(EDGE **edgeRef, double delta, int endFlag)

已确认调用：
sub_140580950(&edge, -0.03, 0)
sub_140580950(&edge, -0.03, 1)
```

本轮实现的是端点向内裁剪后的 legacy interval summary，不是真实 topology trim。

## 新增能力

新增 DTO：

```text
LegacyEdgeEndpointTrim
```

字段：

```text
sourceEdgeStableId
delta
endFlag
trimDistance
originalParameter
trimmedParameter
originalEndpoint
trimmedEndpoint
interval
```

新增 adapter API：

```text
edgeTrimEndpoint(edgeRef, delta, endFlag, sampleCount)
```

当前仅支持旧证据确认的 inward trim：

```text
delta < 0
endFlag = 0 起点
endFlag = 1 终点
```

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- start trim：`delta=-0.03,endFlag=0`，firstParameter 向内移动，lastParameter 保持原终点。
- end trim：`delta=-0.03,endFlag=1`，lastParameter 向内移动，firstParameter 保持原起点。
- near-endpoint no-op trim：过小 delta 稳定拒绝。
- short edge / over-trim：剩余长度低于旧阈值 `0.01` 时稳定拒绝。
- invalid endFlag：稳定拒绝。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: edgeTrimEndpoint 不是 OccLegacyGeometryAdapter 的成员
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
Total Test time = 132.19 sec
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

本轮没有进入 `TODO-012`。

`edgeTrimEndpoint` 是端点 trim summary，不是真实 topology trim、curve mutation 或 spline rebuild。

正向延展 `delta > 0` 未实现，因为当前旧证据只确认 `-0.03` 向内裁剪。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段有两个选择：

```text
TODO-012：组内最小距离检查
```

或：

```text
TODO-020：补 IDA MCP 旧线筋/弧筋链证据
```

如果继续沿 `sub_1405D5670` 的几何能力补齐，建议做 `TODO-012`。如果准备靠近业务算法层，建议先补 `TODO-020`。
