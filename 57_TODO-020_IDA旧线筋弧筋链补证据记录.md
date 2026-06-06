# TODO-020 IDA 旧线筋 / 弧筋链补证据记录

## 本轮目标

本轮完成 `TODO-020 / Evidence`。

目标不是实现线筋 / 弧筋业务代码，而是用 IDA MCP 把旧 VisualTS 的
`sgroupbarline / sgroupbararc / sub_1405D5670` 调用链补到后续
`TODO-021` 可开发的证据级别。

## IDA MCP 状态

本轮 IDA MCP 可用。

```text
idb_list:
  sessions = []

idb_open:
  input_path = ..\VisualTS.exe.i64
  session_id = visualts_i64_todo020
  success = true
  hexrays_ready = true
  auto_analysis_ready = true

survey_binary:
  module = VisualTS.exe
  imagebase = 0x140000000
  total_functions = 17182
  total_strings = 16320
```

因此本轮不是 blocked 节点。

## 关键链路

本轮重新用 IDA MCP 确认的链路如下：

```text
sgroupbarline
  -> sub_1404DE720
  -> sub_1404D10C0
  -> sub_140451730
  -> sub_1405D5670
  -> sub_1405BD0C0
  -> sub_1405C7260
  -> sub_1405E49D0

sgroupbararc
  -> sub_1404DE110
  -> sub_1404D10C0
  -> sub_140451730
  -> sub_1405D5670
  -> sub_1405BD0C0
  -> sub_1405C7260
  -> sub_1405E49D0
```

白话解释：

```text
旧图石不是“OCCT/ACIS 自己创建钢筋”。
旧图石是 VisualTS 业务代码先组织选择对象和 ENTITY_LIST，
再调用 ACIS 做距离、split、spline、trim，
最后把修正后的 EDGE 写回分段钢筋对象并刷新显示 / dirty。
```

## `sub_1404DE720` 线配筋入口

地址：

```text
0x1404DE720
```

本轮确认行为：

- 从当前选择集取对象，要求 `ENTITY_LIST::iteration_count == 1`。
- 要求选中对象通过 `sub_1405C6820` 和 `sub_1405DA020` 校验。
- 要求内部子项数量 `sub_1405D9640(v4) >= 2`。
- 从奇数索引 `1, 3, 5...` 抽取子对象，并把 `child + 80` 的实体加入临时 `ENTITY_LIST`。
- 从首尾相关边读取 4 个端点位置，计算 4 个端点距离。
- 用 `sub_14058F580(4, distances, &minDistance, &minIndex)` 选最小距离。
- 最终调用：

```text
sub_1404D10C0(copiedEntityList, v6, v4, minDistance, selectedEndpointDistance, flag)
```

重要常量 / 字段：

- 初始最小距离候选：`10.0`。
- 对象类型判断读取 `v7 + 176`，并额外检查 `v7 + 200`。
- `flag` 来自对象类型判断，业务名未闭合。

开发含义：

- `Rebar.Create.LineGroup` 不能只做一个参数弹窗。
- 它必须从当前选择对象中提取旧式边链 / 实体列表语义。
- 端点方向和距离会影响后续分段钢筋组创建。

## `sub_1404DE110` 弧形组入口

地址：

```text
0x1404DE110
```

本轮确认行为：

- 从当前选择集取对象，要求 `ENTITY_LIST::iteration_count == 1`。
- 要求选中对象通过 `sub_1405C6820` 和 `sub_1405F17C0` 校验。
- 要求内部元素数量 `sub_1405F25F0(v4) >= 3`。
- 从链式实体关系收集 `ENTITY_LIST`。
- 默认距离参数为 `0.1`。
- 调用 ACIS：

```text
api_entity_entity_distance(entityA, entityB, pointA, pointB, &distance, ...)
```

- 最终调用：

```text
sub_1404D10C0(copiedEntityList, v4, v6, distance, 0.8, 1)
```

重要常量：

- 默认距离：`0.1`。
- 第二距离 / 比例参数：`0.8`。
- flag：`1`。

开发含义：

- `Rebar.Create.ArcGroup` 更接近“链式曲线 / 弧形组”入口。
- `sgroupbararc` 到底对应旧 UI 的 `扇形筋`、`同心圆`，还是二者共用，仍需要旧图石运行确认。

## `sub_1404D10C0` 公共创建事务

地址：

```text
0x1404D10C0
```

原型：

```text
sub_1404D10C0(ENTITY_LIST *entityList,
              __int64 objA,
              __int64 objB,
              double distanceA,
              double distanceB,
              char flag)
```

本轮确认硬条件：

- `objA != null`。
- `objB != null`。
- `sub_1405F25F0(objA) >= 3`。
- `ENTITY_LIST::count(entityList) >= 1`。
- `distanceA >= 0.002`。

本轮确认事务链：

```text
api_bb_begin(1)
  -> copy ENTITY_LIST
  -> sub_140451730(*(objA + 96), copiedEntityList, &createdObject)
  -> sub_1405D5670(objB, *(createdObject + 104), objA)
  -> sub_1405C7260(objB)
  -> vtable_call(..., *(createdObject + 104), 1)
  -> sub_1405E49D0(*(createdObject + 104))
  -> sub_1406B6E20(..., *(createdObject + 112))
  -> sub_1405F4ED0(...)
  -> sub_1405F4A90(...)
  -> sub_1405F5880(...)
  -> sub_1406B2270(...)
api_bb_end(...)
```

重要常量：

- 创建距离下限：`0.002`。
- `distanceA` 会生成 4 位小数截断值。

开发含义：

- 新系统的命令 handler 不能只返回一个 `SteelBarGroup`。
- 创建成功后至少要预留这些事务效果：
  - 领域对象挂接。
  - 几何修正。
  - 显示刷新。
  - dirty 标记。
  - undo / backup。

## `sub_1405D5670` 分段曲线修正

地址：

```text
0x1405D5670
```

IDA 原型：

```text
sub_1405D5670(__int64 objB,
              __int64 createdPayload,
              __int64 objA,
              double thresholdOrDistance)
```

注意：

```text
sub_1404D10C0 里反编译调用只显示 3 个显式实参。
但 Hex-Rays 识别 `sub_1405D5670` 内部使用第 4 个 double。
这个 double 在端部最小距离循环里作为阈值使用。
其真实来源仍是 GAP，不能写成确定业务字段。
```

本轮确认核心流程：

1. 取运行对象链：

```text
createdPayload + 80
objA + 80
objB + 80
entity + 88 / +96 / +72
```

1. 从实体取 ACIS `EDGE`：

```text
api_edge(*(entity + 72), &edge, ...)
```

1. 求当前 edge 到参考实体的最近点 / 距离：

```text
api_entity_entity_distance(edge, *(refNode + 72), pointOnEdge, pointOnRef, &distance, ...)
```

1. 如果最近点不是 edge 起点 / 终点，按最近点 split：

```text
api_split_curve(edge, pointOnEdge, ..., splitEntityList, ...)
```

1. split 后的两段进入长度和端点距离判断：

```text
EDGE::length(partA) >= 0.01
EDGE::length(partB) >= 0.01
distance_to_point(originalStart, originalEnd) <= 0.1
```

1. 近端两段会采样并重建 spline：

```text
sampleCountA = max(5, EDGE::length(partA) * 50)
sampleCountB = max(5, EDGE::length(partB) * 50)
totalPointCount = sampleCountA + sampleCountB - 1
api_curve_spline(totalPointCount, points, ..., &edge, 1, 0, 0)
```

1. 长度足够的边会端部裁剪：

```text
if EDGE::length(edge) > 0.1:
  sub_140580950(&edge, -0.03, endFlag)
```

1. 如果最近点等于端点，则进入端点保护分支：

```text
sub_14058F160(edge, 0.001, ...)
sub_14058F160(edge, 0.999, ...)
api_split_curve(...)
保留较长段
```

1. 写回前还有端部最小距离迭代：

```text
iterationCount = (EDGE::length(edge) - 0.1) / 0.02
while sub_14059B980(group, endpoint) <= threshold:
  sub_140580950(&edge, -0.03, startOrEnd)
```

1. 最终写回：

```text
sub_1405BD0C0(entity, edge)
```

关键常量：

| 常量 | 位置 | 语义 |
|---:|---|---|
| `0.01` | split 后长度判断 | 小段拒绝 / 舍弃阈值 |
| `0.1` | 近端判断 / 端部 trim 前置判断 | 近端、短边和迭代基准 |
| `5.0` | spline 采样数 | 最少采样点 |
| `50.0` | spline 采样数 | 每单位长度采样倍率 |
| `-0.03` | `sub_140580950` | inward trim step |
| `0.001` | 端点保护分支 | 近起点 split 采样比例 |
| `0.999` | 端点保护分支 | 近终点 split 采样比例 |
| `0.02` | 端部最小距离循环 | 最大迭代步数计算 |

开发含义：

- `sub_1405D5670` 应映射为业务层 `SegmentCurveNormalizer`。
- `LegacyGeometryAdapter` 已经具备它需要的大部分几何能力：
  - project point / split at point。
  - endpoint trim summary。
  - point to edge group min distance。
  - spline rebuild summary。
- 后续 `TODO-021` 仍不能直接在业务层调用 OCCT。
  业务层只能调用 legacy DTO / adapter interface。

## 辅助函数补证

### `sub_14058F160`

用途：按比例从 edge 上取点。

确认行为：

- `ratio` 接近 `0` 或 `<= -1.0` 时取 edge 起点。
- `ratio` 接近 `1` 或 `>= 1.0` 时取 edge 终点。
- 中间比例通过 `bounded_curve` 求点。

### `sub_140580950`

用途：按端点方向裁剪 edge。

确认行为：

- 入参为 `EDGE **edgeRef, double delta, int endFlag`。
- `delta >= -0.01` 时直接返回 0。
- 旧调用主要是：

```text
sub_140580950(&edge, -0.03, 0)
sub_140580950(&edge, -0.03, 1)
```

- 内部会通过 bounded curve、split 和必要的 edge 重建实现裁剪。

### `sub_14059B980`

用途：求点到组内实体链的最小距离。

确认行为：

- 从 `group + 88` 开始遍历实体链。
- 每个节点用 `api_entity_point_distance(*(node + 72), point, ..., &distance, ...)`。
- 返回最小距离。

### `sub_1405BD0C0`

用途：把新 edge 写回实体。

反编译核心：

```text
ENTITY::backup(entity)
*(entity + 72) = edge
```

开发含义：

- 写回前有 backup，后续新系统要映射到 undo / transaction，而不是裸覆盖。

### `sub_1405C7260`

用途：刷新父级或关联组。

反编译核心：

```text
result = sub_1405D9660(obj)
if result:
  sub_1405E49D0(result)
```

### `sub_1405E49D0`

用途：显示名 dirty / object tree 刷新。

确认行为：

- 如果 `*(object + 384) == 0`，会进入刷新逻辑。
- 通过对象映射找到显示节点。
- 如果显示名不含 `*`，会在开头插入 `*`。
- 同时处理关联的 `node + 112`。
- 调用 `sub_1405F2710(object, 1)`。

开发含义：

- 旧图石用对象树名字前缀 `*` 表示 dirty 或已修改状态。
- 新系统不必照搬 UI 星号，但必须有等价 dirty 状态和显示刷新事务。

## 本轮结论

`TODO-020` 已把线筋 / 弧筋公共链补到“可以开始 P0 业务创建 spike”的程度。

但它仍不是完整 1:1 复刻闭合：

- `sgroupbararc` 对应旧 UI 的 `扇形筋` / `同心圆` 关系仍需运行确认。
- `sub_1405D5670` 第 4 个 double 的真实来源仍是 GAP。
- `objA / objB / createdPayload` 的业务对象名仍需继续闭合。
- 旧图石失败提示、状态栏提示和参数窗口仍需运行确认。

## 下一步建议

下一阶段可以进入：

```text
TODO-021 / M1-App-020
旧线筋 / 弧筋创建算法 P0 版本
```

前提边界：

```text
只做 P0 业务对象创建 spike。
不直接调用 OCCT。
不迁入父目录 rebar 业务代码。
不声明完整 1:1 golden 已闭合。
```

