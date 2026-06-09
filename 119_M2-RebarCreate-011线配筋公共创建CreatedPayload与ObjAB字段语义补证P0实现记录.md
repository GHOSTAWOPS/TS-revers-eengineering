# M2-RebarCreate-011 线配筋公共创建 CreatedPayload 与 ObjA/B 字段语义补证 P0 实现记录

todoId = TODO-081
phase = M2-RebarCreate-011
evidenceIds = E-IDA-048, E-DEV-103

## 目标

本轮只完成 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0`。

目标不是实现完整线配筋算法，也不是把 `objA / objB` 起一个看起来顺口的中文名，而是继续用 IDA MCP 把旧 `sub_1404D10C0 -> sub_140451730 -> sub_1405D5670` 的对象角色和字段流向收窄到后续可编码边界。

## IDA MCP 会话

```text
database = visualts_todo079
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
```

本轮只读分析：

```text
sub_1404D10C0
sub_140451730
sub_1405D5670
sub_1404DE720
sub_1404DE110
```

## 新增确认

### `sub_1404D10C0`

`sub_1404D10C0` 在 `sub_140451730` 成功后使用 `createdObject` 的两个偏移：

```text
createdPayload = *(createdObject + 104)
linkedModelRef = *(createdObject + 112)
```

已确认调用流：

```text
movsd xmm3, [distanceA_4digit]
sub_1405D5670(objB, *(createdObject + 104), objA, distanceA_4digit)
sub_1405C7260(objB)
vtable_call(runtimeContext, *(createdObject + 104), 1)
sub_1405E49D0(*(createdObject + 104))

sub_1406B6E20(context, *(createdObject + 112))
sub_1405F4ED0(...)
sub_1405F4A90(..., *(createdObject + 112))
sub_1405F5880(...)
sub_1406B2270(context, *(createdObject + 112))
```

因此 `createdObject + 104` 可以作为 `createdPayload` 静态证据继续使用；它参与分段钢筋组几何修正、显示挂接和 dirty 标记。

`createdObject + 112` 只能低置信写成 `linkedModelRef / createdLinkRef`，不能直接命名成旧业务真值。

### `sub_1405D5670` 第 4 个 double

TODO-020 / TODO-080 之前保留了缺口：`sub_1405D5670` 函数原型存在第 4 个 double，但 decompiler 在 `sub_1404D10C0` 调用处只显示 3 个显式实参。

本轮汇编层确认：

```text
1404d1300  movsd xmm3, [rsp+var_1C8]
1404d1306  mov r8, rbx
1404d1309  mov rdx, [rdx+68h]
1404d130d  mov rcx, r15
1404d1310  call sub_1405D5670
```

其中 `[var_1C8]` 是：

```text
distanceA_4digit = (int)(distanceA * 10000.0) / 10000.0
```

所以 `sub_1405D5670` 的第 4 个 double 可静态确认是 `distanceA` 的 4 位截断值，用作后续端部最小距离裁剪阈值。

### `objA / objB` 角色不能写死

线配筋入口：

```text
sub_1404DE720
  -> sub_1404D10C0(entityList, v6, v4, minDistance, selectedEndpointDistance, flag)
```

弧形组入口：

```text
sub_1404DE110
  -> sub_1404D10C0(entityList, v4, v6, distance, 0.8, 1)
```

同一个公共创建函数里：

```text
objA + 96 -> sub_140451730 的创建上下文
objA + 80 -> sub_1405D5670 的一条参考链
objB + 80 -> sub_1405D5670 的一条目标/参考链
objB -> sub_1405C7260 刷新
```

因此本轮只能使用低耦合命名：

```text
objA = createContextOwner / sourceContextObject
objB = refreshTargetObject / counterpartObject
createdPayload = createdObject + 104
linkedModelRef = createdObject + 112
```

不能把 `objA` 永久写成“源钢筋组”，也不能把 `objB` 永久写成“目标钢筋组”。线配筋和弧形组入口已经证明二者会交换。

## 可编码边界

后续 app 代码可以安全表达：

```text
LegacyPublicCreateRoles
  entityListCount
  objAResolved
  objBResolved
  objASub1405F25F0Count
  distanceA
  distanceA4Digit
  distanceB
  flag
  createdPayloadRef
  linkedModelRef
  roleConfidence
```

但本轮没有把它写入代码，因为 TODO-081 是静态证据节点。下一轮如果进入编码，应先补测试，再把这些 role/raw evidence 接到 `RebarGroupCreator` 或更合适的公共创建 DTO。

## 仍未闭合

```text
objA / objB 的旧源码真实类名和 UI 业务名
createdObject + 112 的准确业务含义
object + 80 / 88 / 96 在不同对象上的准确含义
完整 sub_1405D5670 等价算法
旧线配筋真实运行提示、状态栏、主参数窗口字段
线配筋 / 弧形组 golden 对照
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

## 下一步

建议下一轮执行：

```text
TODO-082 / 线配筋公共创建 roles DTO 与 raw evidence 对齐 P0
```

建议原因：

```text
TODO-081 已把 createdPayload、linkedModelRef、distanceA4Digit 和 objA/objB 角色交换风险收窄。
下一轮可以把这些静态证据转成 app 内可测试的角色快照和 raw evidence。
但仍不能进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 或 golden。
```
