# M2-RebarCreate-014 线配筋公共创建 Group-Min / Dirty Gap P0 实现记录

todoId = TODO-084
phase = M2-RebarCreate-014
evidenceIds = E-IDA-022, E-IDA-048, E-IDA-049, E-DEV-105, E-DEV-106

## 目标

本轮只完成 `TODO-084 / 线配筋公共创建 sub_1405D5670 group-min-distance / dirty-write gap 切片 P0`。

目标不是实现完整线配筋算法，也不是在 domain 层复刻 ACIS topology mutation，而是把 TODO-083 刻意保留的三个 gap 拆成正式 app 可测试的 P0 contract：

```text
sub_14059B980 group minimum distance trim loop
sub_1405BD0C0 ENTITY::backup + entity+72 write edge
sub_1405E49D0 dirty write call position
```

能力等级固定为：

```text
p0-gap-contract
```

它只证明旧链路位置、raw evidence、deferred 状态和防误报 gate 已落地，不证明旧 ACIS group list 遍历、真实 edge mutation、旧 undo / dirty / save parity、旧 UI 运行流程或 golden 已闭合。

## IDA MCP 补证

本轮使用 IDA MCP 查询 `VisualTS.exe.i64`：

```text
database = visualts_todo084
```

确认点：

```text
sub_14059B980(group, point)
  -> 读取 group + 88 链表头
  -> 每个节点用 node + 72 作为 ENTITY
  -> 每轮调用 api_entity_point_distance(entity, point)
  -> node + 88 进入下一个节点
  -> 返回最小距离

sub_1405D5670
  -> 0x1405D5F36 调 sub_14059B980 检查 start endpoint
  -> 0x1405D5F86 调 sub_14059B980 检查 end endpoint
  -> 与 arg4 / distanceA_4digit 比较
  -> 距离不大于阈值时调用 sub_140580950 做 endpoint trim

sub_1405BD0C0(entity, edge)
  -> ENTITY::backup(entity)
  -> *((QWORD*)entity + 9) = edge
  -> 即 entity + 72 写入 edge

sub_1404D10C0 成功链
  -> sub_1405D5670
  -> sub_1405C7260
  -> vtable + 0x1C8 显示/挂接调用
  -> sub_1405E49D0(createdObject + 68)
```

`sub_1405E49D0` 的完整 dirty / undo / save 语义仍未闭合；本轮只记录调用位置。

## 代码变更

新增 / 调整：

```text
app/src/domain/rebar/RebarGroupCreator.h
  -> 扩展 LegacySegmentCurveNormalizeTrace
  -> 新增 group-min loop observed / deferred 字段
  -> 新增 backup-write edge observed / deferred 字段
  -> 新增 post-create dirty deferred 字段

app/src/domain/rebar/RebarGroupCreator.cpp
  -> createdFromParameters / legacyRaw 写入 TODO-084 gap contract
  -> 新增 validateP0MutationGapTrace 防误报 gate
  -> reader 若把 group loop / backup write / dirty 标成 applied，creator 直接拒绝
  -> group / bar / segment 保留 unresolved gap

app/tests/unit/rebar_group_creator_tests.cpp
  -> 覆盖 raw evidence、deferred gap、unresolved gap
  -> 覆盖 false applied group-min claim 的 failure diagnostic

app/tests/unit/rebar_line_group_command_handler_tests.cpp
  -> 覆盖 handler 失败不污染 SteelData

app/tests/integration/line_group_display_smoke_tests.cpp
  -> 真实 123.stp 路径断言 TODO-084 raw evidence 写出

tools/phase1_readiness_gate/*
  -> 加入 TODO-084 done report JSON contract
```

## Raw Evidence 字段

`createdFromParameters / legacyRaw` 新增：

```text
sub_14059B980.groupMinimumDistanceTrimLoop
sub_14059B980.groupMinimumDistanceTrimLoop.observed
sub_14059B980.api_entity_point_distance
sub_14059B980.thresholdDistanceA4Digit
sub_14059B980.startEndpointProbe
sub_14059B980.endEndpointProbe
sub_14059B980.startIterationBudget
sub_14059B980.endIterationBudget
sub_1405BD0C0.backupWriteEdge
sub_1405BD0C0.entityBackup
sub_1405BD0C0.entitySlot72Write
sub_1404D10C0.postCreateMutationOrder
sub_1405E49D0.dirtyWrite
```

关键状态值：

```text
sub_14059B980.groupMinimumDistanceTrimLoop = deferred-p0
sub_14059B980.groupMinimumDistanceTrimLoop.observed = true
sub_14059B980.api_entity_point_distance = deferred-acis-group-list
sub_1405BD0C0.backupWriteEdge = deferred-domain-model
sub_1405BD0C0.entityBackup = observed-deferred
sub_1405BD0C0.entitySlot72Write = observed-deferred
sub_1405E49D0.dirtyWrite = deferred-application-state
```

## 防误报 Gate

本轮新增 P0 防线：

```text
groupMinimumDistanceTrimLoopDeferred 必须为 true
backupWriteEdgeDeferred 必须为 true
dirtyWriteDeferred 必须为 true
```

如果后续 reader / adapter / fake test 把这些标成 `applied`，`RebarGroupCreator` 会拒绝并返回 diagnostic。

这条 gate 是为了避免把 TODO-084 的 gap contract 误读成完整旧算法。

## TDD 记录

先补测试后实现：

```text
testLineGroupCreatorRecordsGroupMinimumDistanceDirtyGapContract
  -> red: group-min observed / dirty gap raw fields 不存在
  -> green: raw evidence 和 unresolved gap 写出

testLineGroupCreatorRejectsAppliedGroupMinimumDistanceGapClaim
  -> green: false applied claim 被拒绝，diagnostic 指向 TODO-084 / sub_14059B980

testLineGroupCreatorRejectsAppliedBackupWriteEdgeGapClaim
  -> green: false applied backup/write edge claim 被拒绝，diagnostic 指向 TODO-084 / sub_1405BD0C0 / entity+72

testLineGroupCreatorRejectsAppliedDirtyWriteGapClaim
  -> green: false applied dirty-write claim 被拒绝，diagnostic 指向 TODO-084 / sub_1405E49D0 / dirty

testLineGroupHandlerLeavesModelUntouchedWhenMutationGapClaimIsApplied
  -> green: command 失败不污染 SteelData

line_group_display_smoke_123
  -> green: 真实 123.stp 路径写出 TODO-084 raw evidence
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
修改系统目录
修改 UI 文案
实现完整线配筋算法
实现真实 ACIS group list 遍历
实现真实 topology mutation
实现旧 dirty / undo / save parity
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
让 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## 仍未闭合

```text
完整 sub_1405D5670 topology mutation 等价
sub_14059B980 旧 group list 的真实对象类型和完整遍历语义
sub_1405BD0C0 写回 entity+72 后的完整旧对象生命周期
sub_1405E49D0 的旧 dirty / undo / save parity
createdObject + 112 的准确旧业务含义
objA / objB 的旧源码真实类名和 UI 业务名
旧线配筋真实运行提示、状态栏、主参数窗口字段
线配筋 / 弧形组 golden 对照
```

## 验证

最终验证记录见：

```text
docs/phase1/app_build_reports/m2_rebar_create_014_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_014_run_001.json
```

## 下一步

建议下一轮执行：

```text
TODO-085 / 线配筋命令成功 dirty/transaction 状态 P0
```

下一轮只做新 app 命令成功后的 dirty/transaction P0，不声明旧 `sub_1405E49D0` 完整等价，不进入完整线配筋、面配筋、弧筋、接头、Excel、Detail 或 golden。
