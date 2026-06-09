# M2-RebarCreate-013 线配筋公共创建 Split / Spline / Trim 等价切片 P0 实现记录

todoId = TODO-083
phase = M2-RebarCreate-013
evidenceIds = E-IDA-022, E-IDA-048, E-DEV-104, E-DEV-105

## 目标

本轮只完成 `TODO-083 / 线配筋公共创建 sub_1405D5670 split-spline-trim 等价切片 P0`。

目标不是实现完整线配筋算法，也不是把 OCCT 直接变成钢筋业务层，而是在 TODO-082 已闭合 roles DTO / raw evidence 后，把旧 `sub_1405D5670` 的关键链路拆成正式 app 可测试、可追溯的 P0 trace：

```text
api_entity_entity_distance
  -> api_split_curve
  -> api_curve_spline
  -> sub_140580950 endpoint trim
  -> sub_14059B980 group minimum distance trim loop
  -> sub_1405BD0C0 backup / write edge
```

本轮能力等级固定为：

```text
p0-split-spline-trim-summary
```

它只证明链路 trace、raw evidence 和 adapter summary 边界已接通，不证明真实 ACIS topology mutation、旧 dirty 写回、撤销、保存或 golden 对照已闭合。

## 代码变更

新增 / 调整：

```text
app/src/domain/rebar/RebarGroupCreator.h
  -> 新增 LegacySegmentCurveNormalizeTrace
  -> 新增 LegacySegmentCurveNormalizeResult
  -> LegacyRebarGeometryReader::normalizeSegmentCurve 返回 normalize result + trace

app/src/domain/rebar/RebarGroupCreator.cpp
  -> creationParameters 接收 LegacySegmentCurveNormalizeTrace
  -> createdFromParameters / legacyRaw 写入 split / spline / trim trace 字段
  -> group / bar / segment 都保留 sub_1405D5670.fullEquivalence unresolved

app/src/app/MainWindow.cpp
  -> ViewerLegacyRebarGeometryReader 使用 OccLegacyGeometryAdapter 生成 P0 summary trace
  -> 调用 pointToEdgeGroupDistance / edgeSplitAtPoint / buildSplineFromPoints / edgeTrimEndpoint
  -> OCCT 仍停留在 app/viewer reader 层，没有进入 domain/rebar

app/tests/unit/rebar_group_creator_tests.cpp
  -> 新增 split / spline / trim trace raw evidence 测试
  -> 覆盖 applied / deferred 字段、sample count、segment raw evidence 和 unresolved gap

app/tests/unit/rebar_line_group_command_handler_tests.cpp
  -> 同步 normalizeSegmentCurve 新返回类型
```

## Raw Evidence 字段

`createdFromParameters / legacyRaw` 新增：

```text
sub_1405D5670.normalize.capabilityLevel
sub_1405D5670.api_entity_entity_distance
sub_1405D5670.api_split_curve
sub_1405D5670.api_curve_spline
sub_1405D5670.api_curve_spline.effectiveSampleCount
sub_140580950.startTrim
sub_140580950.endTrim
sub_14059B980.groupMinimumDistanceTrimLoop
sub_1405BD0C0.backupWriteEdge
```

当前 trace 状态值：

```text
applied
attempted-not-applied
not-attempted
deferred-p0
deferred-domain-model
```

其中：

- `groupMinimumDistanceTrimLoop = deferred-p0`
- `backupWriteEdge = deferred-domain-model`

这两个字段刻意不写成 `applied`，因为旧 `sub_14059B980` 的组内循环 trim 和 `sub_1405BD0C0` 的 backup / edge 写回不是本轮 P0 summary 可以证明的内容。

## Adapter 边界

本轮保留正确分层：

```text
domain/rebar
  -> 只看 LegacySegmentCurveNormalizeTrace / Result

app/viewer reader
  -> 调用 LegacyGeometryAdapter

LegacyGeometryAdapter / OccLegacyGeometryAdapter
  -> 内部使用 OCCT 做投影、split、spline summary 和 trim summary
```

因此 `domain/rebar` 仍不包含：

```text
TopoDS_
AIS_
BRep
TopAbs_
OpenCASCADE
opencascade
```

## TDD 记录

先补测试后实现：

```text
新增 testLineGroupCreatorRecordsSplitSplineTrimTraceRawEvidence
  -> red：LegacyRebarGeometryReader 只能返回 curve，无法携带 trace

新增 LegacySegmentCurveNormalizeTrace / Result
  -> green：RebarGroupCreator 可写入 split / spline / trim raw evidence

同步 command handler fake reader 和 MainWindow reader
  -> green：接口编译通过
```

已有 `legacy_geometry_adapter_tests` 继续覆盖 adapter 层：

```text
edgeSplitAtPoint
buildSplineFromPoints
edgeTrimEndpoint start / end
endpoint rejection
near-endpoint no-op trim rejection
wrong type / missing ref diagnostic
```

## 验证

本轮最终验证记录见：

```text
docs/phase1/app_build_reports/m2_rebar_create_013_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_013_run_001.json
```

要求保持：

```text
defaultCTest = pass
readinessGateUnit = pass
readinessGateStrict = pass
domainRebarCommandOCCLeak = pass
gitDiffCheck = pass
xhighReview = allow_commit 或 needs_fix_then_fixed_by_main_flow
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
修改系统目录
修改 UI 文案
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
用 OCCT 直接创建钢筋业务对象
在 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## 仍未闭合

```text
完整 sub_1405D5670 topology mutation 等价
sub_14059B980 group minimum distance trim loop
sub_1405BD0C0 backup / write edge 的真实 dirty 写回等价
createdObject + 112 的准确旧业务含义
objA / objB 的旧源码真实类名和 UI 业务名
旧线配筋真实运行提示、状态栏、主参数窗口字段
线配筋 / 弧形组 golden 对照
```

## 下一步

建议下一轮执行：

```text
TODO-084 / 线配筋公共创建 sub_1405D5670 group-min-distance / dirty-write gap 切片 P0
```

下一轮只继续拆本轮显式 deferred 的 `groupMinimumDistanceTrimLoop` 和 `backupWriteEdge / dirty` 边界；不要在同一轮进入完整线配筋、面配筋、弧筋、接头、Excel、Detail 或 golden。
