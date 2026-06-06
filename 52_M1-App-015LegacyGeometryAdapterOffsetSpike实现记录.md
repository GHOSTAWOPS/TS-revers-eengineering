# M1-App-015 LegacyGeometryAdapter Offset Spike 实现记录

## 结论

本轮完成 `TODO-015 / M1-App-015`。

新增 `LegacyOffsetCurvePreview`：

```text
offsetEdgePreview
```

它对齐旧 VisualTS / ACIS 语义边界：

```text
EDGE
  -> offset distance
  -> curve offset preview
  -> samplePoints / length / bbox / diagnostic
```

本轮实现的是 offset 曲线 preview summary / 能力 spike，不是钢筋排布业务，
也不是 OCCT / ACIS topology mutation 等价实现。

## 新增能力

新增 DTO：

```text
LegacyOffsetCurvePreview
```

字段：

```text
sourceEdgeStableId
offsetDistance
requestedSampleCount
effectiveSampleCount
sourceCurveKind
sourceLength
length
bounds
samplePoints
offsettable
failureReason
```

新增 adapter API：

```text
offsetEdgePreview(edgeRef, offsetDistance, sampleCount)
```

当前语义：

```text
LegacySelectionRef(edge)
  -> validate distance / sample count / ref type
  -> resolve OCCT edge inside adapter
  -> choose stable normal from source tangent and global axis fallback
  -> build Geom_OffsetCurve preview
  -> sample preview points
  -> return length / bbox / stable diagnostic
```

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
```

已覆盖：

- 显式寻找一条 line edge，offset preview 成功。
- 成功结果保留 source stableId、offset distance、sample count、curve kind。
- 成功结果返回 samplePoints、positive length 和 bbox。
- 非 line edge 要么成功返回 preview，要么稳定返回 failure DTO，不崩溃。
- distance 为 `0`、`infinity`、`NaN` 时稳定拒绝。
- wrong type / missing ref 返回稳定 diagnostic。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: offsetEdgePreview 不是 OccLegacyGeometryAdapter 的成员
```

GREEN 窄测试：

```text
cmd /c call "D:\Visual Studio 2026\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build build --target legacy_geometry_adapter_tests && build\legacy_geometry_adapter_tests.exe "C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp"
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 169.93 sec
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
Minor = `OccLegacyGeometryAdapter.cpp` 中 `effectiveSampleCount == 1` 分支不可达；
        因为前面已固定为 `std::max(2, sampleCount)`，不影响当前提交。
Verdict = allow_commit
```

xhigh 子代理已在 review 结束后关闭。

## IDA 状态

本轮检查 IDA MCP：

```text
idb_list
sessions = []
count = 0
```

因此本轮没有新增 IDA 闭环证据。旧偏移需求仍按既有文档作为能力
spike 输入：

```text
15_线配筋与弧形组专项初稿.md
16_seg_steelbargroup字段地图初稿.md
35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md
46_CSE_v2Goal执行目标与Todo说明.md
```

该缺口继续保留在 `99_缺口和待确认项.md`。

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

本轮没有进入 `TODO-016`。

`offsetEdgePreview` 是 edge ref 到 offset curve preview 的 summary，不等价于
OCCT wire / edge topology mutation，也不等价于旧 ACIS offset API 的完整行为。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段建议做：

```text
TODO-016：LegacyGeometryAdapter section / 剖切能力 spike
```

原因：

```text
剖切 / 线面求交 / section 是剖面圈筋、工程图割切面和后续钢筋检查的
关键底层能力。offset preview 已完成后，下一步补 section 更贴近旧图石
钢筋与工程图交叉流程。
```

也可以先补：

```text
TODO-020：IDA MCP 旧线筋 / 弧筋链证据
```

如果 IDA 数据库已重新绑定，建议优先用它闭合旧业务调用链。
