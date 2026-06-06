# M1-App-017 LegacyGeometryAdapter Sweep Boundary 实现记录

## 结论

本轮完成 `TODO-017 / M1-App-017`。

新增 `LegacyCircularSweepPreview`：

```text
edgeCircularSweepPreview
```

它对齐旧 VisualTS / ACIS 的能力边界：

```text
EDGE path
  -> circular profile radius
  -> OCCT pipe / sweep preview inside adapter
  -> shape summary / samplePoints / bbox / diagnostic
```

本轮实现的是 sweep / pipe preview summary / 能力边界验证，不是钢筋业务，
不是工程图业务，也不是 ACIS topology mutation 的完整等价实现。

## 新增能力

新增 DTO：

```text
LegacyCircularSweepPreview
```

新增 adapter API：

```text
edgeCircularSweepPreview(edgeRef, radius, sampleCount)
```

当前语义：

```text
LegacySelectionRef(edge)
  -> validate edge ref / radius / sample count
  -> resolve OCCT edge inside adapter
  -> sample source edge centerline
  -> build circular profile at path start
  -> run BRepOffsetAPI_MakePipe
  -> return pathLength / bbox / samplePoints / face-edge-vertex count / diagnostic
```

返回字段：

```text
sourceEdgeStableId
radius
requestedSampleCount
effectiveSampleCount
sourceCurveKind
pathLength
bounds
samplePoints
shapeFaceCount
shapeEdgeCount
shapeVertexCount
sweepable
failureReason
```

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
sha256 = 6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
```

已覆盖：

- 直线 edge + 合法半径返回 sweepable summary。
- summary 保留 source edge stableId、radius、curve kind、pathLength、samplePoints 和 bbox。
- summary 返回 swept shape 的 face / edge / vertex 计数。
- 非直线 edge 要么成功返回 summary，要么稳定返回 failure DTO。
- 半径为 0 稳定拒绝。
- 非有限半径稳定拒绝。
- sampleCount 为 0 稳定拒绝。
- wrong type / missing ref 返回稳定 diagnostic。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: edgeCircularSweepPreview is not a member of OccLegacyGeometryAdapter
```

GREEN 窄测试：

```text
cmd.exe /c 'call "D:\Visual Studio 2026\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build app\build --target legacy_geometry_adapter_tests && app\build\legacy_geometry_adapter_tests.exe "C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp"'
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 130.88 sec
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

xhigh 只读 review：

```text
Critical = none
Important = none
Minor =
  - effectiveSampleCount == 1 branch is unreachable because effectiveSampleCount = max(2, sampleCount).
  - tests do not separately assert shapeVertexCount.
Verdict = allow_commit
```

xhigh 子代理已在 review 结束后关闭。

## IDA 状态

本轮没有新增 IDA 闭环证据。

原因：

```text
TODO-017 是 OCCT sweep / pipe 能力边界验证。
它只证明新 adapter 内部可把 OCCT sweep 包装成 legacy summary。
旧图石钢筋实体生成规则、profile 半径来源、端部处理、导出策略仍需后续 IDA 或旧图石运行确认。
```

## 修改范围

```text
app/CMakeLists.txt
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 边界

本轮没有实现钢筋生成。

本轮没有迁移父目录钢筋业务主链。

本轮没有实现真实钢筋实体持久化、STEP 导出、工程图输出或 Detail writer。

`edgeCircularSweepPreview` 是 edge ref + radius 到 circular sweep preview 的
summary，不等价于 OCCT shape mutation 对业务层开放，也不等价于旧 ACIS
sweep / pipe API 的完整行为。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段建议做：

```text
TODO-018：钢筋领域模型冻结 P1
```

原因：

```text
LegacyGeometryAdapter 的首批 EDGE / FACE / distance / split / trim / spline /
wire chain / offset / section / sweep 能力边界已经形成。继续堆 adapter spike
收益开始下降，下一步应该把 SteelBar / SteelBarSegment / SteelBarGroup 字段
按旧证据冻结，准备让业务层开始创建可追溯的钢筋对象。
```

如果 IDA 数据库恢复绑定，也可以先补：

```text
TODO-020：IDA MCP 旧线筋 / 弧筋链证据。
```
