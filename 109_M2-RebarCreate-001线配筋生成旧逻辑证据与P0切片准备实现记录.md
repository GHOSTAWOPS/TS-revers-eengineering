# M2-RebarCreate-001 线配筋生成旧逻辑证据与 P0 切片准备实现记录

todoId = TODO-071
phase = M2-RebarCreate-001
evidenceIds = E-IDA-045, E-DEV-093

## 目标

本轮只完成 `TODO-071 / 线配筋生成旧逻辑证据与 P0 切片准备`。

目标不是完整实现线配筋，也不是用 OCCT 直接生成钢筋，而是回到旧 VisualTS 证据，确认 `sgroupbarline` 的旧入口契约，并把这个契约固化到当前 `RebarGroupCreator` P0 输出中。

## IDA MCP 确认

本轮 IDA MCP 打开：

```text
database = visualts_todo071_line_rebar
input = VisualTS.exe.i64
hexrays_ready = true
```

已确认：

```text
0x14095B440 table entry:
  command string -> 0x140768A08 = "sgroupbarline"
  handler        -> 0x1404DE720 = sub_1404DE720
  secondary      -> 0x14054B410
```

`sub_1404DE720` 确认：

```text
selection count must be 1
selected object must pass sub_1405C6820 and sub_1405DA020
internal child count must be >= 2
odd-index child entities are extracted into ENTITY_LIST
four endpoint distance candidates are calculated
initial minimum distance candidate is 10.0
calls sub_1404D10C0(entityList, objA, objB, minDistance, selectedEndpointDistance, flag)
```

`sub_1404D10C0` 确认：

```text
requires objA / objB / entityList
requires sub_1405F25F0(objA) >= 3
requires entityList count >= 1
requires distanceA >= 0.002
enters ACIS api_bb_begin(1)
calls sub_140451730 to create / attach payload
calls sub_1405D5670 for seg_steelbargroup split / spline / trim chain
refreshes parent / display / dirty state through sub_1405C7260 and sub_1405E49D0
```

`sub_1405D5670` 仍保持 TODO-020 结论：

```text
api_entity_entity_distance -> api_split_curve -> api_curve_spline
minimum split length = 0.01
near endpoint distance = 0.1
trim delta = -0.03
spline samples = max(5, EDGE::length * 50)
endpoint guard ratios = 0.001 / 0.999
trim iteration step = 0.02
```

## 代码切片

修改文件：

```text
app/src/domain/rebar/RebarGroupCreator.cpp
app/tests/unit/rebar_group_creator_tests.cpp
```

实现内容：

```text
RebarGroupCreator::createLineGroup
  -> createdFromParameters / legacyRaw 增加 sgroupbarline 入口契约字段
  -> group.evidence 增加 E-IDA-045
  -> group.unresolvedLegacyFields 保留 selectionObjectType 低置信缺口
```

新增字段：

```text
sgroupbarline.selectionCount = 1
sgroupbarline.selectionGate.structureCandidate = sub_1405C6820
sgroupbarline.selectionGate.payloadCandidate = sub_1405DA020
sgroupbarline.minimumInternalItemCount = 2
sgroupbarline.oddIndexedEntityExtraction = child-index-1-3-5-...
sgroupbarline.endpointDistanceCandidateCount = 4
sgroupbarline.initialMinimumDistanceCandidate = 10.0
sgroupbarline.publicCreateCall =
  sub_1404D10C0(entityList,objA,objB,minDistance,selectedEndpointDistance,flag)
```

## TDD

红灯：

```text
rebar_group_creator_tests failed:
line group must carry TODO-071 sgroupbarline entry evidence
```

绿灯：

```text
rebar_group_creator_tests = pass
```

## 验证

```text
targeted build = pass
targeted test = pass; rebar_group_creator_tests
CTest = pass; 18/18 pass; total 157.10 sec
domain/rebar OCCT leak scan = pass; no TopoDS_ / AIS_ / BRep / TopAbs_ matches
readiness gate unit = pass; 31/31 pass
strict readiness gate = pass; M1-Formal-Ready; 84/84 pass
git diff --check = pass
xhighReview = needs_fix; important findings fixed by main agent; agent closed
```

## 明确不证明

```text
不证明线配筋完整 1:1。
不证明旧 UI 选择流程 / 状态栏提示 / 参数窗口已运行确认。
不证明 objA / objB / createdPayload 的业务名已经闭合。
不证明 RebarGroupCreator 已等价 sub_1405D5670 完整算法。
不证明 golden、AutoCAD L2、面配筋、接头或 Excel。
```

## 下一步

建议下一步只进入：

```text
TODO-072 / 线配筋命令 handler P0：LegacySelection 到 RebarGroupCreator 事务接入
```

边界：

```text
继续只通过 LegacyGeometryAdapter / LegacyRebarGeometryReader 拿几何语义。
不让 domain/rebar 或 command handler 直接依赖 TopoDS / AIS / BRep / TopAbs。
不迁入父目录 rebar 业务代码。
```
