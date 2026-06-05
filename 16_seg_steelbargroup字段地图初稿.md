# seg_steelbargroup 字段地图初稿

## 目标

本文件记录 `seg_steelbargroup` 相关 IDA 证据。

用途：

- 支撑 `线配筋`、`扇形筋`、`同心圆` 的 1:1 功能复刻。
- 把旧 ACIS 里的曲线处理规则翻译成新系统的钢筋业务层算法。
- 给后续 Qt6 + OCCT 开发提供可编码的字段和函数边界。

当前状态：

```text
已确认核心算法链。
已确认若干结构偏移和几何规则。
字段业务名仍是初稿，不能当成最终 SFL 字段表。
```

## 入口链路

线配筋和弧形组共用底层生成链：

```text
sgroupbarline -> sub_1404DE720
sgroupbararc  -> sub_1404DE110
  -> sub_1404D10C0
  -> sub_140451730
  -> sub_1405D5670
  -> sub_1405BD0C0
  -> sub_1405C7260
  -> sub_1405E49D0
```

关键确认：

- `sub_1405D5670` 命中源码路径：

```text
e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

- 这条链不是普通显示逻辑，而是分段钢筋组的几何生成 / 修正逻辑。

## 函数职责地图

### `sub_1404D10C0`

初步职责：线/弧钢筋组公共创建事务。

已确认行为：

- 校验 `objA`、`objB`、实体列表数量。
- 要求 `sub_1405F25F0(objA) >= 3`。
- 要求传入实体列表数量不少于 1。
- 要求距离 `a4 >= 0.002`。
- 将 `a4` 格式化并按 4 位小数截断。
- 进入 ACIS `api_bb_begin(1)` block。
- 复制实体列表。
- 调用 `sub_140451730(*(_QWORD *)(objA + 96), copiedEntityList, &createdObject)`。
- 创建成功后调用：

```text
sub_1405D5670(objB, *(_QWORD *)(createdObject + 104), objA)
sub_1405C7260(objB)
vtable_call(..., *(_QWORD *)(createdObject + 104), 1)
sub_1405E49D0(*(_QWORD *)(createdObject + 104))
```

可编码含义：

- 这是一个命令事务边界。
- 新系统应把它对应到 `Rebar.Create.LineGroup` / `Rebar.Create.ArcGroup` 的 command handler。
- 几何生成成功后必须同时做：领域对象挂接、显示刷新、dirty 标记、撤销记录。

### `sub_1405D5670`

初步职责：分段钢筋组曲线修正和边重建。

函数签名：

```text
sub_1405D5670(__int64 objB, __int64 createdPayload, __int64 objA, double trimDistance)
```

IDA 反编译显示只有 3 个显式实参调用，但函数原型识别出第 4 个 double。
因此 `trimDistance` 的来源和默认值仍需继续确认。

已确认行为：

- 从 `createdPayload + 80`、`objA + 80`、`objB + 80` 取链表或组节点。
- 从节点 `+88` 取当前实体。
- 用 `api_edge(entity + 72, &edge)` 取 ACIS `EDGE`。
- 计算当前边到参考实体的距离：

```text
api_entity_entity_distance(edge, *(_QWORD *)(refNode + 72), ...)
```

- 若距离点不是当前 edge 的端点，则对 edge 执行：

```text
api_split_curve(edge, distancePoint, ...)
```

- 分裂后若得到两段，按长度和端点距离选择保留段或重建段。
- 当两段端点距离小于等于 `0.1` 时，会把两段采样点合并成 spline：

```text
api_curve_spline(pointCount, points, ..., &edge, ...)
```

- 对结果边两端做迭代裁剪，直到端点到已有组的最小距离大于阈值。
- 最后通过 `sub_1405BD0C0(entity, edge)` 把新边写回实体。

可编码含义：

- 这是“钢筋线/弧段生成后的几何清理”。
- 不是 UI 功能，也不是 OCCT 自动能替代的业务规则。
- 新系统应把它实现为 `RebarSegmentGeometryNormalizer` 一类业务算法。

### `sub_1405BD0C0`

初步职责：把新的 ACIS `EDGE` 写回实体。

反编译：

```text
ENTITY::backup(a1);
*((_QWORD *)a1 + 9) = a2;
```

确认偏移：

- `entity + 72`：当前几何实体指针，旧系统里常作为 ACIS `EDGE` 取用。

可编码含义：

- 修改前先备份，说明这是可撤销/可回滚的实体几何变更。
- 新系统应对应为：

```text
oldSegment.curve = newCurve
commandJournal.recordGeometryChange(oldCurve, newCurve)
```

### `sub_1405C7260`

初步职责：从对象找到上级/关联对象后刷新。

反编译：

```text
result = sub_1405D9660(a1);
if (result)
  return sub_1405E49D0(result);
```

可编码含义：

- 生成段后不只刷新当前对象，还要刷新其父组或关联对象。
- 新系统应在 `RebarGroup` 级别刷新显示、统计和脏状态。

### `sub_1405D9660`

初步职责：通过 `a1 + 80` 找到父节点/关联节点。

反编译：

```text
node = *(_QWORD *)(a1 + 80);
if (node)
  return sub_1405BC270(node);
```

可编码含义：

- `+80` 很可能是对象挂接链表 / 树节点入口。
- 需要继续追 `sub_1405BC270` 才能确定是父组、拥有者还是显示树节点。

### `sub_1405E49D0`

初步职责：刷新显示树/对象名，并设置更新标记。

已确认行为：

- 若 `a1 + 384` 为 0，则继续处理。
- 取全局文档/视图对象。
- 找到 `a1` 对应的显示/树对象。
- 读取名称字符串，若不包含 `*`，就在名称前插入 `*`。
- 调用 vtable `+888` 更新显示名称。
- 调用 `sub_1405C6EE0` 写回名称。
- 调用 `sub_1405F2710(a1, 1)` 设置状态。
- 对关联对象 `v5 + 112` 执行同样的 `*` 标记逻辑。

可编码含义：

- `*` 很像旧图石对象树里的“已修改/dirty”前缀。
- `a1 + 384` 可能是锁定、隐藏、已删除或不需要更新的状态位，业务名未闭合。
- 新系统至少要有：

```text
object.dirty = true
group.dirty = true
treeItem.setModified(true)
viewer.refresh(object, group)
```

### `sub_140580950`

初步职责：按端点方向裁剪/延展 edge，并生成新线段。

函数签名：

```text
sub_140580950(EDGE **edgeRef, double delta, int endFlag)
```

已确认调用：

```text
sub_140580950(&edge, -0.03, 0)
sub_140580950(&edge, -0.03, 1)
```

可编码含义：

- `delta = -0.03` 表示向内裁剪一小段。
- `endFlag = 0/1` 表示裁剪起点或终点。
- OCCT 可用曲线参数范围、端点切向和 `Geom_TrimmedCurve` 等方式替代。

### `sub_14058F160`

初步职责：按曲线参数取点。

函数签名：

```text
sub_14058F160(SPAposition *out, EDGE *edge, double normalizedParam, bounded_curve *curve)
```

已确认调用：

```text
sub_14058F160(out, edge, 0.001, 0)
sub_14058F160(out, edge, 0.999, 0)
sub_14058F160(out, edge, i / (count - 1), 0)
```

可编码含义：

- 旧系统用接近 0 和 1 的参数避开端点奇异问题。
- 新系统采样时不要直接默认端点永远稳定，应保留 `0.001 / 0.999` 这类保护策略。

### `sub_14059B980`

初步职责：求点到钢筋组内所有边的最小距离。

已确认行为：

- 从 `a1 + 88` 开始遍历链表。
- 对每个节点的 `node + 72` 调用：

```text
api_entity_point_distance(nodeEdge, point, ...)
```

- 返回最小距离。

可编码含义：

- 用于判断新边端点是否离已有组太近。
- 对应新系统的 `minDistance(point, group.curves)`。

### `sub_1405DC840`

初步职责：取链表末尾节点。

反编译：

```text
result = *(_QWORD *)(a1 + 88);
while (result && *(_QWORD *)(result + 88))
  result = *(_QWORD *)(result + 88);
return result;
```

可编码含义：

- `+88` 在这类对象中很可能是同级 next 指针。
- 线/弧组算法会根据链表首尾关系决定从哪段开始处理。

## 结构偏移初稿

以下字段名是新系统开发用的临时命名，不等于旧源码真实字段名。

| 偏移 | 初步命名 | 证据 | 置信度 |
|---:|---|---|---|
| `+72` | `entityGeometry` / `edgeHandle` | `api_edge(*((_QWORD*)entity+9))`；`sub_1405BD0C0` 写入 | 高 |
| `+80` | `ownerNode` / `attachNode` | 多处以 `object + 80` 进入链表或父级查找 | 中 |
| `+88` | `nextEntity` / `childOrNext` | `sub_1405DC840` 顺链表遍历；`sub_14059B980` 遍历组内边 | 中 |
| `+96` | `nextRefNode` / `nextGroupNode` | `sub_1405D5670` 循环中 `v6 = *(v6 + 96)` | 低 |
| `+104` | `createdPayload` | `sub_1404D10C0` 从 `createdObject + 104` 传入 `sub_1405D5670` | 中 |
| `+112` | `linkedDisplayOrModel` | `sub_1405E49D0` 对 `v5 + 112` 关联对象加 `*` | 低 |
| `+384` | `skipDirtyOrLockedFlag` | `sub_1405E49D0` 仅在该 DWORD 为 0 时刷新 | 低 |

注意：

- 本表来自运行期算法链，描述的是 `sub_1405D5670` 等函数处理的对象上下文。
- 后续 save/read 证据显示，`seg_steelbargroup` 持久化字段 `+72` 是字符串。
- 因此不能把本表的 `entity + 72` 几何用法直接等同于 SFL 持久化字段 `+72`。
- 新格式中持久化段名写入 `legacyObject.raw.string72DisplayName`，几何边写入 `geometryRef.curveRefs`，二者对应关系写入 `binding.items`。

## 保存函数补证

关键函数：

```text
sub_1405DA720
```

已确认 `seg_steelbargroup` 写出顺序：

```text
write_id_level("seg_steelbargroup", 2, 0)
sub_1406A2320(a1, entityList)
write_int(a1 + 68)
write_string(a1 + 72)
write_ptr(a1 + 80)
write_ptr(a1 + 96)
write_ptr(a1 + 88)
write_int(a1 + 64)
```

开发含义：

- 保存顺序不是偏移升序，`+96` 在 `+88` 前写出。
- 新设计文件格式必须保存 `legacyWriteOrder`，不能只按字段名排序。
- `+68` 高概率对应样本里的 `segIndex`。
- `+72` 高概率对应样本里的 `段_N` 字符串。
- `+80/+96/+88/+64` 业务名仍未闭合，只能暂存为 raw 字段。
- 已补到对应 restore/read 函数，见下节。

## 读取函数补证

关键函数：

```text
sub_1405DA5D0
```

包装入口：

```text
sub_1405D5600
```

包装入口会分配 `112` 字节对象，源码路径命中：

```text
e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

已确认读取顺序：

```text
sub_1406A22C0(a1)
read_int()    -> a1 + 68
read_string() -> a1 + 72
read_ptr()    -> a1 + 80
read_ptr()    -> a1 + 96
read_ptr()    -> a1 + 88
read_int()    -> a1 + 64
```

结论：

- `seg_steelbargroup` 的持久化 save/read 顺序已经成对确认。
- 持久化 `+72` 是字符串字段，对应样本里的 `段_N`。
- 早期算法链中看到的 `entity + 72` 几何边用法，不能直接和持久化 `+72` 合并解释。
- 当前更稳妥的写法是：保存层使用 `legacyObject.raw.string72DisplayName`，几何层使用 `geometryRef.curveRefs`，绑定层使用 `binding.items` 记录二者的角色关系和证据。
- `ptr80/ptr96/ptr88/int64RoleOrState` 的业务名仍未闭合。

## 几何规则初稿

已确认常量：

- 最小创建距离：`0.002`。
- split 后边长有效阈值：`0.01`。
- 判断两段端点接近阈值：`0.1`。
- 端部裁剪步长：`-0.03`。
- 裁剪循环步长分母：`0.02`。
- spline 采样数：`max(5, EDGE::length(edge) * 50)`。
- split 内部避开端点参数：`0.001`、`0.999`。

算法白话版：

```text
1. 对每段旧边，先找它和参考边/参考对象的最近点。
2. 最近点如果在边中间，就把边切开。
3. 切开后保留有效长段；如果两段端点很近，就把两段重新采样并拟合成一条样条边。
4. 对新边两端一点点往内裁，直到端点和已有钢筋组拉开足够距离。
5. 把修正后的边写回当前钢筋段。
6. 刷新父组、显示树和 dirty 状态。
```

## OCCT 替代点

旧 ACIS 能力和 OCCT 对应关系：

| 旧 ACIS 调用 | 旧用途 | OCCT 替代方向 |
|---|---|---|
| `api_edge` | 从实体取 EDGE | 领域对象直接保存 `TopoDS_Edge` / curve handle |
| `api_entity_entity_distance` | 边到实体距离 | `BRepExtrema_DistShapeShape` 或曲线距离封装 |
| `api_split_curve` | 按点切边 | 曲线参数定位 + `Geom_TrimmedCurve` / 拆 `TopoDS_Edge` |
| `EDGE::length` | 取边长 | `GCPnts_AbscissaPoint::Length` / `BRepGProp` |
| `api_curve_spline` | 点列拟合样条边 | `GeomAPI_PointsToBSpline` + `BRepBuilderAPI_MakeEdge` |
| `api_entity_point_distance` | 点到组内边最小距离 | `GeomAPI_ProjectPointOnCurve` / `BRepExtrema` |

开发提醒：

- OCCT 负责几何能力替代。
- 旧图石的阈值、采样、裁剪、组刷新规则要在旧图石证据驱动的钢筋复刻层实现。
- 不要把这部分写成“OCCT 自动完成线配筋”。
- 实现前必须回到旧图石函数证据、字段地图和运行确认，不允许按 OCCT 能力重新设计规则。

## 新系统建议类边界

建议先按下面几个类切开，不急着做大而全抽象：

```text
RebarGroupCommandHandler
  -> LineGroupCreator
  -> ArcGroupCreator
  -> SegmentCurveNormalizer
  -> SegmentGeometryStore
  -> RebarDirtyMarker
```

当前确认职责：

- `LineGroupCreator`：处理 `sgroupbarline` 等价选择和距离输入。
- `ArcGroupCreator`：处理 `sgroupbararc` 等价选择和弧形边输入。
- `SegmentCurveNormalizer`：复刻 `sub_1405D5670` 的 split / spline / trim。
- `SegmentGeometryStore`：保存 `edgeHandle` 等价几何。
- `RebarDirtyMarker`：复刻 `sub_1405E49D0` 的 dirty 和显示树刷新。

## 与 SFL 的关系

当前只能确认：

- SFL 样本里有 `seg_steelbargroup`。
- IDA 里有 `seg_steelbargroup.cpp`。
- IDA 已定位 `sub_1405DA720` 保存函数。
- 二者高度相关。

## SFL 可见字段补充证据

对当前 6 个 `.sfl` 样本做只读扫描后，`seg_steelbargroup` 记录头可见骨架基本稳定：

```text
seg_steelbargroup
  hcc
  ref(-1)
  i32(-1)
  i32(classCode)
  i32(segIndex)
  str("段_N")
  ref1/ref80
  ref2/ref96
  ref3/ref88
  i32(tailInt)
```

注意：上面的 `ref1/ref2/ref3` 是样本扫描时的可见顺序暂名。
IDA 保存函数已确认真实偏移顺序为 `+80 -> +96 -> +88`。
后续字段命名必须以 IDA 保存/读取证据为准。

6 个样本的 `seg_steelbargroup` 数量：

| 样本 | 数量 |
|---|---:|
| 消力池下游侧带齿槽底板结构图石钢筋模型.sfl | 16 |
| 尾水调压室闸墩结构图石钢筋模型_替换前.sfl | 306 |
| 尾水调压室闸墩钢筋模型（2705m以上）_校审后修改.sfl | 429 |
| 4#塔图石模型7.16最终.sfl | 512 |
| 边墩1107（底板+边墩配筋+接头坝段)替换模型后-未合并钢筋-修改廊道位置-替换后.sfl | 561 |
| 14#坝段表孔闸墩及流道2877.00m高程以上钢筋图.sfl | 681 |

可见字段初判：

| 可见字段 | 暂定命名 | 初判含义 | 置信度 |
|---|---|---|---|
| `segIndex` | `segmentIndex` | 与 `段_N` 完全对应 | 高 |
| `str("段_N")` | `displayName` | 分段显示名 / 对象树名 | 高 |
| `ref(-1)` / `i32(-1)` | `emptyRefOrDefaultFlag` | 通用空引用或默认状态 | 中 |
| `classCode` | `sflClassCode` | SFL 类版本 / 保存格式码 | 中低 |
| `ref1` | `primarySegmentOrGeometryRef` | 可能关联相邻段或几何线 | 低 |
| `ref2` | `secondaryRef` | 可选第二关联对象 | 低 |
| `ref3` | `ownerInstanceRef` | 多数疑似指向 `pj_instace` | 中 |
| `tailInt` | `segmentRoleOrState` | 段类型 / 端点角色 / 状态位 | 低 |

分布线索：

- `classCode` 已见：`837 / 839 / 840 / 842 / 843 / 844 / 845`。
- `tailInt` 只见：`0 / 1 / 2 / 3`。
- `ref2` 既可能是 `-1`，也可能是非负对象引用。
- `segIndex` 最大值在 14#坝段样本达到 `79`。
- `seg_steelbargroup` 后常接 `pj_instace` 或 `__layer`。
- `steelbar` 可见名称类似 `1根钢筋 / 2根钢筋 / 3根钢筋`。
- `steelbargroup` 可见名称类似 `组_1 / 组_N`。

注意：

```text
SFL 可见字段是文件层证据。
IDA 的 +72/+80/+88 等是运行内存对象偏移。
二者高度相关，但当前不能直接一一映射。
```

仍不能直接确认：

- SFL `seg_steelbargroup` 每个字段的保存顺序。
- `+72/+80/+88/...` 是否和 SFL 字段一一对应。
- 旧 SFL 中 `steelbar`、`steelbargroup`、`seg_steelbargroup` 的完整父子关系。

因此本文件只作为“运行内存对象字段地图初稿”，不能替代 SFL 读写格式说明。

## 待继续确认

P0：

- `sub_1405D5670` 第 4 个 double 参数的来源。
- `createdObject + 104`、`createdObject + 112` 的业务对象名。
- `object + 80/88/96` 在不同对象上的准确含义。

P1：

- `sub_1405BC270` 的返回对象是否是父组。
- `sub_1405C6EE0` 是否只改显示名，还是也写业务名。
- `sub_1405F2710(a1, 1)` 的状态位含义。
- `sub_1405F4ED0 / sub_1405F4A90 / sub_1405F5880` 是否是显示选择和 dirty 操作。
- SFL `ref1/ref2/ref3/tailInt` 的真实字段名和保存函数。

P2：

- SFL `seg_steelbargroup` 字段保存顺序。
- 线配筋和弧形组在旧 UI 中的实际失败提示。
- 旧图石对象树里 `*` 的完整语义。

## 当前结论

`seg_steelbargroup` 的核心不是简单字段表，而是一套“分段曲线修正算法”。

最快复刻时，不能只按 UI 参数生成直线/圆弧。
必须把以下规则先做成业务层 spike：

- 最近点 split。
- 小段过滤。
- 近端两段 spline 重建。
- 端部 `-0.03` 逐步裁剪。
- 组内最小距离判断。
- 写回段几何并刷新父组 dirty。
