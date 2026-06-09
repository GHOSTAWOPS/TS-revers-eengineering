# M2-RebarCreate-010 线配筋公共创建 Core 参数 Gate 与 Diagnostic 对齐 P0 实现记录

todoId = TODO-080
phase = M2-RebarCreate-010
evidenceIds = E-IDA-045, E-IDA-047, E-DEV-102

## 目标

本轮只完成 `TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0`。

目标不是补完整线配筋算法，也不是猜旧 UI 文案，而是把旧 `sub_1404D10C0` 已确认的公共创建前置 gate 落到正式 `app` 的 P0 测试、实现和 raw evidence 中。

## 证据边界

旧证据已经确认：

```text
sub_1404D10C0(entityList, objA, objB, distanceA, distanceB, flag)

objB must be non-null
objA must be non-null
sub_1405F25F0(objA) >= 3
ENTITY_LIST count >= 1
distanceA >= 0.002
```

本轮只把这些已确认 gate 做成 P0 兼容快照：

```text
LegacyPublicCreateGateSnapshot
  objAResolved
  objBResolved
  objASub1405F25F0Count
  entityListCount
```

默认值表示当前 P0 单 edge 代用输入可通过：

```text
objAResolved = true
objBResolved = true
objASub1405F25F0Count = 3
entityListCount = 1
```

这不是说旧 `objA / objB / ENTITY_LIST` 的完整业务名已经闭合。

## 实现内容

本轮修改：

```text
app/src/domain/rebar/RebarGroupCreator.h
app/src/domain/rebar/RebarGroupCreator.cpp
app/tests/unit/rebar_group_creator_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
```

新增 / 调整：

```text
LegacyPublicCreateGateSnapshot

sub_1404D10C0 gate diagnostic:
  objB missing
  objA missing
  sub_1405F25F0(objA) < 3
  ENTITY_LIST count < 1
  distanceA < 0.002

createdFromParameters raw fields:
  sub_1404D10C0.objB.present
  sub_1404D10C0.objA.present
  sub_1404D10C0.objA.sub_1405F25F0.count
  sub_1404D10C0.objA.sub_1405F25F0.minimum
  sub_1404D10C0.entityList.count
  sub_1404D10C0.entityList.minimum
```

新增测试覆盖：

```text
missing objB -> reject before geometry query
missing objA -> reject before geometry query
sub_1405F25F0(objA) < 3 -> reject before geometry query
ENTITY_LIST count < 1 -> reject before geometry query
distanceA < 0.002 -> diagnostic includes sub_1404D10C0 and 0.002
success path -> raw fields record old gate snapshot
```

readiness gate 已增加 `TODO-080` done-report 映射和单测，防止缺实现记录 / run report 时误把节点标为 done。

## 验证

本轮已运行：

```text
cmake --build .\app\build --target rebar_group_creator_tests
ctest --test-dir .\app\build -R rebar_group_creator_tests --output-on-failure
  -> pass

cmake --build .\app\build
ctest --test-dir .\app\build --output-on-failure
  -> 21/21 pass
```

后续最终收口还需要：

```text
readiness gate strict
domain/rebar + command OCCT leak check
git diff --check
xhigh read-only review
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
objA / objB / createdPayload 的最终业务名
完整 ENTITY_LIST 输入对象语义
sub_140451730 创建对象字段语义
完整 sub_1405D5670 等价
旧线配筋真实运行提示、状态栏、主参数窗口字段
完整线配筋算法、undo/dirty、golden
```

继续保留在：

```text
GAP-IDA-002
GAP-IDA-007
GAP-UI-REB-001
GAP-REB-C-002
```

## 下一步

下一轮建议执行：

```text
TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0
```

建议原因：

```text
TODO-080 只把公共创建 gate 落到 P0 代码。
如果直接继续写完整线配筋算法，objA / objB / createdPayload 字段语义仍会变成猜测。
下一步应优先用 IDA MCP 继续追 sub_1404D10C0 -> sub_140451730 -> createdObject / payload 字段，
把旧业务对象语义补到可继续编码的程度。
```
