# IDA 命令证据

## 说明

本文件记录 IDA / 二进制确认的旧命令字符串、命令表项和入口函数。

命令表结构当前判断为：

```text
命令字符串地址
  -> 入口函数
  -> 可用性/状态函数
```

表项通常以 3 个 qword 为一组。

## 旧英文命令字符串

IDA `find_regex` 已命中以下旧命令字符串：

| 旧命令 | 字符串地址 | 初步语义 |
|---|---:|---|
| `barjointnew` | `0x140768480` | 新建接头/搭接 |
| `barjointclear` | `0x140768490` | 清除接头 |
| `barjointmove` | `0x1407684A0` | 移动接头 |
| `barjointrev` | `0x1407684B0` | 接头反向 |
| `barmove` | `0x140768828` | 钢筋移动 |
| `steelhide` | `0x140768870` | 隐藏/显示层相关 |
| `steeldis` | `0x140768880` | 显示钢筋 |
| `steeldiam` | `0x1407688B0` | 钢筋属性/直径 |
| `sgroupbararc` | `0x1407689F8` | 弧形钢筋组 |
| `sgroupbarline` | `0x140768A08` | 线性钢筋组 |
| `sbararc2circle` | `0x140768D00` | 弧钢筋转圆/圆处理 |
| `smove` | `0x140768D78` | 钢筋 / 钢筋组移动，语义待继续确认 |
| `scopy` | `0x140768D80` | 钢筋拷贝 |
| `smovenext_seg` | `0x140768D98` | 下一段移动，语义待继续确认 |
| `srev_steelbar` | `0x140768DA8` | 钢筋反向 |
| `sexportbar` | `0x140768DE0` | 导出钢筋 |
| `checkoverlapsteel` | `0x140768E00` | 钢筋碰撞/重叠检查 |
| `checkmindststeel` | `0x140768E18` | 最小钢筋间距 |
| `checkgmindststeel` | `0x140768E30` | 组内/箍筋最小间距 |
| `rebars` | `0x140768E44` | 批量钢筋入口 |
| `rebar` | `0x140768E4C` | 普通钢筋入口 |
| `rebarz` | `0x140768ECC` | 语义待确认 |
| `rebarpost` | `0x140768ED8` | 后处理钢筋入口，语义待确认 |
| `ysteelout` | `0x140769008` | 钢筋输出，语义待确认 |
| `refl_createSteel` | `0x1407692A0` | 参考对象创建钢筋 |
| `modify_refl_Steel` | `0x1407692B8` | 修改参考驱动钢筋 |

## 已解析命令表项

| 旧命令 | 表项地址 | 入口函数 | 状态/可用性函数 | 新系统建议命令 |
|---|---:|---:|---:|---|
| `sgroupbararc` | `0x14095B428` | `sub_1404DE110` | `sub_14054AE90` | `Rebar.Create.ArcGroup` |
| `sgroupbarline` | `0x14095B440` | `sub_1404DE720` | `sub_14054B410` | `Rebar.Create.LineGroup` |
| `sexportbar` | `0x14095B638` | `sub_1404D1540` | `sub_14045A020` | `Rebar.Export.SteelStepOrLegacy` |
| `barmove` | `0x14095B230` | `sub_1404D5040` | `0` | `Rebar.Edit.Move` |
| `scopy` | `0x14095ADF8` | `sub_1405A9DD0` | `0` | `Rebar.Edit.Copy` |
| `steelhide` | `0x14095B4E8` | `sub_1404DA2C0` | `0` | `Rebar.View.Hide` |
| `steeldis` | `0x14095B500` | `sub_1404D50F0` | `0` | `Rebar.View.Show` |
| `steeldiam` | `0x14095B548` | `sub_140538DB0` | `0` | `Rebar.Edit.Properties` |
| `checkoverlapsteel` | `0x14095BF68` | `sub_14059BB80` | `0` | `Rebar.Check.Overlap` |
| `checkmindststeel` | `0x14095BF80` | `sub_14059BB00` | `0` | `Rebar.Check.MinDistance` |
| `checkgmindststeel` | `0x14095BF98` | `sub_14059BA80` | `0` | `Rebar.Check.GroupMinDistance` |
| `rebars` | `0x14095AD20` | `sub_1405ADF00` | `0` | `Rebar.Create.BatchFromSelection` |
| `rebar` | `0x14095AAE0` | `sub_14055C1F0` | `sub_14054AE20` | `Rebar.Create.FromSelection` |
| `rebarz` | `0x14095AC18` | `sub_14055C3E0` | `sub_14054AE20` | `Rebar.Create.ZOrPostGroup` |
| `rebarpost` | `0x14095AC30` | `sub_14055C310` | `sub_14054AE20` | `Rebar.Create.PostProcess` |
| `ysteelout` | `0x14095B968` | `sub_140600890` | `sub_14054AC70` | `Rebar.Export.YSteelOut` |
| `refl_createSteel` | `0x14095BC38` | `sub_14055B540` | `0` | `Rebar.Create.FromReference` |
| `modify_refl_Steel` | `0x14095BC50` | `sub_14055C110` | `0` | `Rebar.Edit.ReferenceDriven` |

## `barmove` / `Input_Choice` 移动链补证

证据编号：

- `E-IDA-023`

入口：

```text
barmove -> sub_1404D5040
```

当前 IDA MCP 结论：

- `sub_1404D5040` 要求当前选择集数量至少为 1。
- 选中对象先过 `sub_1405BC870` 类型检查。
- 入口取选中包装对象 `+80` 指针，放入临时 `ENTITY_LIST`。
- 调用：

```text
sub_14058B770(&byte_140762208, &byte_1407621F8, selectedList, 0, -2)
```

可确认含义：

- 传入 `Input_Choice` 的 copy flag 为 `0`，对应移动原对象，不是复制。
- 字符串块含 `钢筋移动`、`选择移动方式`、`Z向移动距离(毫米)`、`平移距离`。
- `sub_14058B770` 分配并显示 `Input_Choice` 窗口，Dialog ID 为 `384`。

`Input_Choice` 关键字段：

```text
a1 + 1072 / 1080 / 1088 = 三个 double 输入字段
a1 + 1096 = 份数 / 次数字段，DDV 范围 1..1000
a1 + 1136 = 第一选择 ENTITY_LIST
a1 + 1144 = 第二选择 ENTITY_LIST
a1 + 1152 = 移动方式 mode
a1 + 1160 = SPAtransf
a1 + 1272 = 初始选中钢筋实体列表
a1 + 1280 = copy flag
```

`sub_1404EF8B0` 负责根据 mode 生成 `SPAtransf`：

- mode 4：要求两个 `VERTEX`，由第二点减第一点生成平移向量，再 `translate_transf`。
- mode 5：由 `a1+1080 / a1+1088 / a1+1072` 构造 `SPAvector`，再 `translate_transf`。
- 其他 mode 会生成 reflect / rotate 等变换，当前不作为 `Rebar.Edit.Move P0` 实现范围。

`sub_1404F1170` 是执行入口：

- `UpdateData(TRUE)` 后调用 `sub_1404EF8B0` 生成 `SPAtransf`。
- 复制 `a1+1272` 中的选中实体列表到临时列表。
- 进入 ACIS bulletin board 事务：`api_bb_begin -> update_from_bb -> api_bb_end`。
- copy flag 为 `0` 时走移动原对象路径，调用 `sub_1405AA5D0(entityList, ..., SPAtransf, 0)`。
- 局部分支还会调用 `sub_140447210`，该函数内部继续调用 `sub_1405AA5D0`，并对 pattern / 子实体调用 `ENTITY::backup`。

当前可开发级结论：

- `Rebar.Edit.Move P0` 可先实现“选中钢筋组整体平移，保留 group / bar / segment 身份”的领域语义。
- 领域层不得复刻 ACIS topology mutation；只能移动 domain `SteelBarSegment` 的起点 / 中点 / 终点，并保留 binding / geometryRef / evidence。
- dirty、撤销、旧提示文本、reflect/rotate 等其他 mode 仍是后续 GAP。

## `scopy` / `Input_Choice` 拷贝链补证

证据编号：

- `E-IDA-024`

本轮复核时重开后的 IDA MCP 会话：

```text
session = visualts_i64_todo032
database = VisualTS.exe.i64
Hex-Rays = ready
```

命令字符串和表项：

```text
scopy string = 0x140768D80

xrefs:
  0x140959EC8
  0x14095ADF8

handler table:
  0x14095ADF8 -> 0x140768D80 "scopy"
  0x14095AE00 -> sub_1405A9DD0
  0x14095AE08 -> 0
```

入口链：

```text
scopy
  -> sub_1405A9DD0
  -> sub_14058B770(byte_1407768E8, byte_1407768D8, selectedList, 1)
  -> sub_1404EED00 / Input_Choice
  -> sub_1404EF8B0
  -> sub_1404F1170
  -> sub_1405989C0
  -> sub_1405AA5D0
```

`sub_1405A9DD0` 当前确认：

- 要求当前选择集数量 `>= 1`。
- 遍历当前选择集。
- 选中对象先过 `sub_1405C6820` 校验。
- 选中对象 `+13` 指向对象继续过 `sub_1405F17C0` 校验。
- 要求该对象 `+80` 非空。
- 把符合条件的选中对象加入临时 `ENTITY_LIST`。
- 对选中对象调用 `sub_1405C25D0(entity, 1)` 做状态 / 选择标记。
- 最终调用：

```text
sub_14058B770(byte_1407768E8, byte_1407768D8, copiedEntityList, 1)
```

可确认含义：

- `scopy` 使用 Dialog `#384` 对应的 `Input_Choice` 窗口。
- 传入 `Input_Choice + 1280` 的 copy flag 为 `1`。
- 与 `barmove` 的 copy flag `0` 区分明确：`0` 移动原对象，`1` 走复制后变换路径。

`sub_1404EED00 / Input_Choice` 本轮复核：

- `a1 + 1096` 初始值为 `1`，对应份数 / 次数字段。
- `a1 + 1152` 为移动 / 拷贝方式 mode。
- `a1 + 1160` 为 `SPAtransf`。
- `a1 + 1272` 为初始选中实体列表。
- `a1 + 1280` 为 copy flag。

`sub_1404F1170` copy flag `1` 分支当前确认：

- 先调用 `sub_1404EF8B0` 生成 `SPAtransf`。
- 把 `Input_Choice + 1272` 的选中实体复制到临时 `ENTITY_LIST`。
- 进入 `api_bb_begin -> update_from_bb -> api_bb_end` ACIS bulletin board 事务。
- 当 `copy flag = 1` 时，对每个源实体按 `a1 + 1096` 循环多份：

```text
sub_1405989C0(sourceEntity, clonedEntityList, 0)
SPAtransf *= originalTransform  // 第 2 份起叠加变换
sub_1405AA5D0(clonedEntityList, mode, accumulatedTransform, 0)
```

工程含义：

```text
旧 scopy 的主线不是直接移动原对象。
它先复制源钢筋组 / 子对象 / edge 链，再把复制件按 Input_Choice 得到的变换应用出去。
多份拷贝时，第 N 份使用重复 N 次的同一变换。
```

`sub_1405989C0` 当前确认：

- 如果输入是包装对象且类型为 `110`，取其 `+13` 指向的内部对象。
- 调用 `sub_140447D90` 创建新 group / 容器类对象。
- 调用 `sub_1405E79E0`、`sub_1405F2B60` 等复制 / 挂接组信息。
- 遍历源对象 `+80/+88` 链。
- 通过 `ACIS_OBJECT::operator new(..., "e:\\tushi3d\\dam\\steel\\qtool.cpp")` 和 `sub_1405DB340` 创建新的 `steelbar` 一类对象。
- 对源 edge 调用：

```text
api_edge(source + 72, &edge)
sub_14054BD40(edge, newBar, 0)
ENTITY::backup(...)
```

- 写回复制件的 `+80/+88/+96/+104` 等引用链，并把新对象加入输出 `ENTITY_LIST`。

当前可开发级结论：

- `Rebar.Edit.Copy P0` 可以实现为“复制已选 domain group / bar / segment，然后对复制件按累计平移向量变换”。
- P0 必须保持源对象不变，新增 group / bar / segment 身份，并保证引用关系一致。
- 因旧编号、`rsdID`、`displayNumber`、legacy raw 写回、dirty / undo / 运行提示仍未闭合，新系统 P0 不应继承源对象的旧编号和 legacy raw 字段。
- 新系统 P0 复制件的 `binding / geometryRef` 应标为需要重新绑定，不能假装已拥有旧 ACIS / STEP 几何引用。

本轮不能过度推断：

- 旧图石复制后 `groupID / rsdID / displayNumber / actualNumber / sequenceNo` 的完整生成规则未闭合。
- `sub_1405989C0` 内部 `+80/+88/+96/+104` 等引用链业务名仍未完全闭合。
- dirty / undo / 保存提示 / 旧错误提示文本仍需旧图石运行确认。
- P0 中 `copyCount 1..1000` 只按当前 `Input_Choice` 字段和已有 DDV 线索作为安全边界；旧提示文本和所有 UI 分支仍未闭合。

## `sgroupbarline` 初步反编译

入口：

```text
sgroupbarline -> sub_1404DE720
```

当前 IDA MCP 初步结论：

- 函数要求当前选择集数量为 1。
- 先校验选中对象类型。
- 继续校验选中对象内部结构，要求子项数量不少于 2。
- 从对象结构中按奇数索引抽取一组实体，放入临时 `ENTITY_LIST`。
- 读取两个端点对象的坐标，计算四种端点距离。
- 取最小距离和对应端点距离。
- 最后调用公共生成函数：

```text
sub_1404D10C0(entityList, v6, v4, minDistance, selectedDistance, flag)
```

可确认的几何含义：

- 该入口不像简单 Dialog 弹窗，而是基于当前选择对象生成线性钢筋组。
- 计算逻辑依赖 ACIS `ENTITY_LIST` 和 `SPAposition`。
- `sub_1404D10C0` 很可能是线/弧组共同的钢筋组创建核心。

仍未确认：

- `v4`、`v6` 的业务对象名。
- `flag` 的业务意义。
- UI 上它具体对应 `线配筋` 的哪种选择流程。

## `sgroupbararc` 初步反编译

入口：

```text
sgroupbararc -> sub_1404DE110
```

当前 IDA MCP 初步结论：

- 函数要求当前选择集数量为 1。
- 校验选中对象类型。
- 校验选中对象内部结构，要求元素数量不少于 3。
- 从链式实体关系中收集一组实体到临时 `ENTITY_LIST`。
- 调用 ACIS `api_entity_entity_distance` 计算两个实体之间的距离。
- 默认距离参数初始值为 `0.1`。
- 最后调用公共生成函数：

```text
sub_1404D10C0(entityList, v4, v6, distance, 0.8, 1)
```

可确认的几何含义：

- 该入口处理的是弧形/环形一类钢筋组，不是普通面配筋窗口。
- 它和 `sgroupbarline` 共用 `sub_1404D10C0`。
- `0.8` 可能是默认比例/容差/偏移参数，语义未确认。

仍未确认：

- 它在 UI 上具体对应 `扇形筋`、`同心圆`，还是二者共用的一条底层入口。
- `sub_1405F17C0` / `sub_1405F25F0` 校验的对象类型业务名。
- `sub_1404D10C0` 生成对象的字段含义。

## 线/弧钢筋组公共生成链

入口：

```text
sgroupbarline -> sub_1404DE720 -> sub_1404D10C0
sgroupbararc  -> sub_1404DE110 -> sub_1404D10C0
```

`sub_1404D10C0` 原型：

```text
sub_1404D10C0(ENTITY_LIST *entityList, __int64 objA, __int64 objB, double distanceA, double distanceB, char flag)
```

当前 IDA MCP 初步结论：

- 该函数会校验 `objA`、`objB`、实体列表数量和距离下限。
- 距离小于 `0.002` 时直接失败。
- 会格式化距离参数。
- 会进入 ACIS API block：

```text
api_bb_begin(1)
ACISExceptionCheck("API")
```

- 会复制 `ENTITY_LIST` 并调用：

```text
sub_140451730(*(_QWORD *)(objA + 96), copiedEntityList, &createdObject)
```

- 如果生成成功，会调用：

```text
sub_1405D5670(objB, createdObjectPayload, objA)
sub_1405C7260(objB)
sub_1405E49D0(createdObjectPayload)
```

其中 `sub_140451730` 作用：

- 复制/重建传入实体列表。
- 将实体挂接到当前模型/对象映射。
- 返回新生成对象。

其中 `sub_1405D5670` 作用：

- IDA 反编译命中源码路径：

```text
e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

- 这说明该函数属于旧图石 `seg_steelbargroup` 分段钢筋组逻辑。
- 函数内部调用 ACIS：

```text
api_edge
api_entity_entity_distance
api_split_curve
api_curve_spline
```

- 函数包含边长判断、曲线分割、端部裁剪、样条曲线生成。
- 常量和规则片段包括：
  - 边长阈值 `0.01`。
  - 端点距离阈值 `0.1`。
  - 曲线裁剪步长 `-0.03`。
  - 采样数量约为 `EDGE::length * 50`，且至少 `5`。

工程含义：

```text
线配筋 / 弧形组不是简单画几根线。
旧图石在 seg_steelbargroup.cpp 中做曲线拆分、端部处理和分段组生成。
新系统要把这类逻辑作为钢筋业务层复刻，OCCT 只替代 ACIS 的曲线/距离/分割能力。
```

后续应继续追：

- `sub_1405D5670` 中 `a1/a2/a3` 的业务对象名。
- `sub_1405BD0C0` 是否把生成边写入 steelbar/segment。
- `sub_1405C7260` 是否刷新钢筋组。
- `sub_1405E49D0` 是否刷新显示或保存 dirty。
- `seg_steelbargroup` 与 SFL `seg_steelbargroup` 字段的对应关系。

## 检查命令闭合证据

已确认：

```text
checkoverlapsteel -> sub_14059BB80 -> Dialog #449 钢筋碰撞检查
checkmindststeel  -> sub_14059BB00 -> Dialog #450 平行钢筋最小距离
checkgmindststeel -> sub_14059BA80 -> Dialog #450 平行钢筋最小距离
```

## TODO-071 线配筋入口契约 IDA MCP 复核

Evidence ID：

- `E-IDA-045`

本轮 IDA MCP 重新打开：

```text
VisualTS.exe.i64
session_id = visualts_todo071_line_rebar
hexrays_ready = true
```

本轮复核 `sgroupbarline` 表项：

```text
0x14095B440:
  0x140768A08 -> "sgroupbarline"
  0x1404DE720 -> sub_1404DE720
  0x14054B410 -> secondary handler / helper
```

`sub_1404DE720` 本轮再次确认：

```text
selection count == 1
selected entity passes sub_1405C6820
selected payload passes sub_1405DA020
internal child count >= 2
odd-index child entities are extracted into a temporary ENTITY_LIST
four endpoint distance candidates are calculated
initial minimum distance candidate = 10.0
sub_14058F580 selects the minimum distance candidate
sub_1404D10C0(entityList, objA, objB, minDistance, selectedEndpointDistance, flag)
```

工程含义：

```text
Rebar.Create.LineGroup 的业务入口不能简化成“用户选一条 OCCT Edge 就生成钢筋”。
当前 P0 可把单 edge 作为 spike 输入，但必须在领域对象 raw/evidence 中保留旧入口契约。
```

仍未闭合：

```text
旧 UI 选择对象的业务名。
旧运行状态栏提示。
objA / objB / createdPayload 的最终业务名。
sub_1405D5670 第 4 个 double 的真实调用来源。
```

## TODO-076 线配筋旧 UI 失败提示与状态栏口径静态 stop point

Evidence ID：

- `E-IDA-046`

IDA MCP 会话：

```text
database = visualts_todo076
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

本轮补看的函数：

```text
sub_1404DE720     // sgroupbarline handler
sub_14054B410     // secondary / availability helper
sub_1404D10C0     // line / arc group common creation core
sub_14058B8D0     // Input_float modal wrapper
sub_1404F5120     // Input_float constructor
sub_1406B7FE0     // common outcome / part note state path
sub_1406B8140     // api_part_start_state wrapper
sub_14054C720
sub_14054C760
```

字符串扫查：

```text
pattern = 线配筋|请选择|选择.*边|边.*选择|钢筋组|状态栏|失败|参数|距离
matches = 0
```

### `sub_1404DE720` 失败路径

本轮确认该 handler 内部没有直接中文提示字符串，也没有直接 `MessageBox / AfxMessageBox / SetPaneText` 类调用。

失败分支基本是前置 gate 后直接 `return 0`：

```text
selection count != 1                         -> return 0
sub_1405C6820(selected) fail                 -> return 0
sub_1405DA020(selected+13) fail              -> return 0
child count < 2                              -> return 0
sub_1405BC270(obj+80) null                   -> return 0
first child +96 null                         -> return 0
```

继续通过后，函数仍是旧入口契约主线：

```text
odd-index child entities -> temporary ENTITY_LIST
4 endpoint distance candidates
initial minimum distance candidate = 10.0
sub_1404D10C0(entityList, objA, objB, minDistance, selectedEndpointDistance, flag)
```

### `sub_14054B410` 可用性 / 状态 helper

本轮确认该 helper 也没有直接提示字符串。

当前最稳妥伪代码：

```text
ctx = sub_1406ED3C0(a1)
if ctx && selection_count >= 1:
  iterate selection list
  every selected entity must:
    pass sub_1405C6820
    and sub_1405C6F90(entity) == 120
  all pass -> return 1
return 0
```

工程含义：

```text
它更像命令启用 / 可用性判断，
不能证明旧线配筋失败提示或状态栏文案。
```

### `sub_1404D10C0` 公共创建链

本轮确认 `sub_1404D10C0` 仍只有公共创建 / ACIS outcome 路径，没有线配筋专属中文失败提示。

关键路径：

```text
validate objA / objB / ENTITY_LIST / distance
distance < 0.002 -> fail
sub_14045BEF0(Buffer, ..., a4)
roundedDistance = int(a4 * 10000.0) / 10000.0
sub_14058B8D0(titleOrLabel, labelOrTitle, &roundedDistance)
api_bb_begin
ACISExceptionCheck("API")
sub_140451730(...)
sub_1405D5670(...)
sub_1405C7260(...)
sub_1405E49D0(...)
problems_list_prop::process_result(...)
sub_1406B7FE0(...)
```

本轮只看到字符串：

```text
"API"
```

这说明失败 / 状态处理主要进入 ACIS `outcome` 和公共 `problems_list_prop / part_note_state` 链路，但该静态路径仍未给出旧图石线配筋用户可见提示文本。

### `Input_float / Dialog #383`

`sub_14058B8D0` 会构造并 `DoModal` 一个公共浮点输入窗口：

```text
sub_14058B8D0(char *title, char *label, double *value)
  -> sub_1404F5120(stackObject, *value, title, label, 0)
  -> CDialog::DoModal()
  -> OK 后写回 double
```

`sub_1404F5120` 确认：

```text
CDialog::CDialog(..., 0x17F, ...)
0x17F = 383
source path = e:\tushi3d\dam\tool\input_float.cpp
a1 + 304 = double value
a1 + 320 = copied title string
a1 + 328 = copied label string
```

工程含义：

```text
Dialog #383 是公共 Input_float 浮点输入窗。
它可以证明旧公共创建链里存在距离 / 数值确认步骤，
但不能等同于旧线配筋主参数窗口字段清单。
```

### 公共状态路径

`sub_1406B7FE0` 调用：

```text
api_part_note_state
api_part_name_state
ha_rendering_context::GetGeomPattern
view/model refresh-like call
```

`sub_1406B8140` 只是：

```text
api_part_start_state(...)
```

二者都是大量 ACIS API 调用共用的 outcome / part state 路径，当前没有线配筋专属状态栏文案证据。

### 本轮 stop point

已确认：

```text
sgroupbarline handler 内没有直接中文失败提示字符串。
secondary helper 内没有直接中文失败提示字符串。
common creation core 没有线配筋专属提示字符串，只有公共 ACIS outcome 链。
common creation core 会打开 Input_float Dialog #383。
Dialog #383 是公共浮点输入窗，不是旧线配筋主参数窗口字段清单。
直接中文字符串扫查未命中线配筋 / 请选择 / 状态栏 / 失败 / 参数等候选。
```

仍未闭合：

```text
旧线配筋运行时失败提示。
旧线配筋状态栏 pane 文案。
旧线配筋完整参数 Dialog 字段、默认值、单位、灰显状态。
旧 UI 点击流程中 #383 的标题和标签实参最终中文。
旧 UI 选择对象业务名。
```

后续建议：

```text
TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0
```

`Dialog #449` 字段：

- 集合1。
- 集合2。
- 碰撞结果。
- 执行。
- 关闭。

`Dialog #450` 字段：

- 钢筋距离。
- 钢筋1。
- 钢筋2。
- 单位 mm。

## 导出命令证据

Evidence ID：

- `E-IDA-014`

`sexportbar` 表项：

```text
0x14095B638: 0x140768DE0 -> "sexportbar"
0x14095B640: 0x1404D1540 -> 入口函数
0x14095B648: 0x14045A020 -> 状态/可用性函数
```

`ysteelout` 表项：

```text
0x14095B968: 0x140769008 -> "ysteelout"
0x14095B970: 0x140600890 -> 入口函数
0x14095B978: 0x14054AC70 -> 状态/可用性函数
```

旧导出字符串证据含：

```text
Rebar:[Count%d]
@Group:[Code%d],[Diameter%d],[Grade%d],[Usage%d],[Segs%d],[Counts%d]
SteelBar:[Seg%d]
$L(%g,%g,%g;%g,%g,%g)
```

运行补证：

- `E-RUN-002`：用户截图确认 `输出钢筋` 位于 `开始 / 文件` 分组。
- 用户运行确认旧图石 `输出钢筋` 可导出钢筋 `.stp`。
- 当前只能确认“旧图石存在钢筋 STP 导出能力”，尚未确认 `.stp` 是 `sexportbar` 还是 `ysteelout` 触发。
- 后续需要保留导出样本，并用 OCCT STEP 导入统计 roots / solids / faces / edges / curves。
- `Rebar/Group/SteelBar` 文本结构和 `.stp` 导出可能是同一入口的不同格式选项，也可能是不同命令链；未闭合前不得合并成确定结论。

## 旧钢筋对象保存字段证据

Evidence ID：

- `E-IDA-015`

本轮用 IDA MCP 追了旧对象字符串和保存函数，确认以下旧图石对象写出骨架。

### `steelData`

字符串和源码路径：

```text
steelData
e:\tushi3d\dam\class\steeldata.cpp
```

关键函数：

```text
sub_14045CCF0
```

已确认写出：

- `write_id_level("steelData", 2, 0)`
- `write_string(*(a1 + 64))`
- `a1 + 72/76/80` 三个 int
- `a1 + 84..120` 十个数值字段，版本 `<831` 写 int，`>=831` 写 real
- `a1 + 124..140` 五个 int
- `a1 + 144/148` 两个 logical
- 版本 `>=838` 时按 flag 追加 `a1 + 152/156`
- 版本 `>=841` 时追加 `a1 + 160/164`

结论：

```text
steelData 保存字段骨架已确认。
字段业务名未闭合，不能直接命名成材料/级别/弯钩等确定语义。
```

### `steelData` 读取函数补证

关键函数：

```text
sub_14045C940
```

已确认读入：

- `read_string` 后写入 `a1 + 64`。
- `a1 + 72/76/80` 三个 int。
- `a1 + 84..120` 十个数值字段，版本 `<831` 读 int 转 float，`>=831` 读 real。
- `a1 + 124..140` 五个 int。
- `a1 + 144/148` 两个 logical。
- 版本 `>=838` 时按 flag 读取 `a1 + 152/156`。
- 版本 `>=841` 时读取 `a1 + 160/164`。

结论：

```text
steelData 的 save/read 顺序成对确认。
新设计文件格式可以按 legacyObject.raw 保存 typed fields。
字段业务名仍未闭合，不能直接提升为确定业务字段。
```

### `steelbar`

字符串和源码路径：

```text
steelbar
e:\tushi3d\dam\class\steelbar.cpp
```

关键函数：

```text
sub_1405E1C10
```

已确认写出：

- `write_id_level("steelbar", 2, 0)`
- `write_string(*(a1 + 64))`
- `a1 + 72/80/88` 三个对象引用
- `a1 + 96/100` 两个 int

结论：

```text
steelbar 保存骨架很短，关键是三个引用的业务含义。
当前不能直接确认它们分别对应 group / segment / style。
```

补充：

```text
sub_1405E0EA0
```

该函数创建 `DEBUG_LIST("steelbar")`，并引用源码路径：

```text
e:\tushi3d\dam\class\steelbar.cpp
```

它是类注册 / 调试列表证据，不是字段读取函数。

### `steelbar` 读取函数补证

关键函数：

```text
sub_1405E1AB0
```

包装入口：

```text
sub_1405DF3F0
```

包装入口行为：

- `ACIS_OBJECT::operator new(136, ..., "e:\tushi3d\dam\class\steelbar.cpp")`
- 调用 `sub_1405DB340(v0, 0)` 初始化对象。
- 调用 `sub_1405E1AB0(v1)` 读取字段。

已确认读入：

- `read_string` 后写入 `a1 + 64`。
- `a1 + 72/80/88` 三个 `read_ptr`。
- `a1 + 96/100` 两个 `read_int`。
- 读完后旧代码把 `a1 + 120 = 0`、`a1 + 104 = 0`。

结论：

```text
steelbar 的 save/read 顺序成对确认。
持久化 raw 字段可按 +64/+72/+80/+88/+96/+100 保存。
但 +72/+80/+88/+96/+100 的业务名仍未闭合。
```

### `steelbargroup`

字符串和源码路径：

```text
steelbargroup
e:\tushi3d\dam\class\steelbargroup.cpp
```

关键函数：

```text
sub_1405F2100
```

已确认写出：

- `write_id_level("steelbargroup", 2, 0)`
- `write_string(*(a1 + 72))`
- `a1 + 80/88/96/104/112` 五个对象引用
- `a1 + 120/128/136` 等基础 int / 版本化字段
- `a1 + 144` vector
- `a1 + 168/192` 两个 position
- `a1 + 224/240/248/256/280..320` 多个 real
- `a1 + 364/368/372/380` logical / flag 字段
- `a1 + 328/332/336/340/344/348/352/356/360` 一组 int 字段
- 版本 `>=842` 写 `a1 + 384`
- 版本 `==805` 写 `a1 + 388`
- 名称包含 `Varies_ShangH_LZ` 时写额外 logical、两个 vector、一个 int 和两个字符串列表

结论：

```text
steelbargroup 是大字段对象。
保存骨架已确认，但大多数字段业务名还不能确定。
新设计文件格式应先保存 typed legacy fields，再逐步提升为业务字段。
```

### `steelbargroup` 读取函数补证

关键函数：

```text
sub_1405F1A90
```

已确认读入：

- `read_string` 后写入 `a1 + 72`。
- `a1 + 80/88/96/104/112` 五个 `read_ptr`。
- `a1 + 120` int。
- 版本 `<807` 从 real 米制读入并换算到 `a1 + 128`，否则读 int。
- `a1 + 136` int。
- `a1 + 224/240` real。
- `a1 + 168/192` 两个 position。
- `a1 + 144` vector，读入后 normalise。
- `a1 + 220/132` 两个 int。
- `a1 + 248/256/280..320` 多个 real。
- `a1 + 364/368/372/380` logical / version branch。
- `a1 + 328/332/336/340/344/348/352/356/360` 编码型 int 字段。
- 版本 `>=842` 读 `a1 + 384`。
- 版本 `==805` 读 `a1 + 388`。
- 名称包含 `Varies_ShangH_LZ` 时读额外 logical、两个 unit_vector、`a1 + 216` 和两个字符串列表。

特别规则：

- `a1 + 328` 读入值 `>2` 时，旧代码设置 `a1 + 340 = 1`，并把 `a1 + 328` 减 `3`。
- `a1 + 332` 读入值 `>2` 时，旧代码设置 `a1 + 344 = 1`，并把 `a1 + 332` 减 `3`。
- 版本 `>=838` 时，上述两个分支还会读取 `a1 + 348/352`。
- 版本 `>=839` 读取 `a1 + 356/360`。

结论：

```text
steelbargroup 的 save/read 顺序成对确认。
但 +328/+332/+340/+344/+348/+352 等仍只能写成编码型 legacy raw 字段。
它们很可能和编号或首末增减根数有关，需运行属性窗口或保存前后差异闭合。
```

### `seg_steelbargroup` 保存函数补证

关键函数：

```text
sub_1405DA720
```

已确认写出：

- `write_id_level("seg_steelbargroup", 2, 0)`。
- `a1 + 68` int。
- `a1 + 72` string。
- `a1 + 80` ptr。
- `a1 + 96` ptr。
- `a1 + 88` ptr。
- `a1 + 64` int。

结论：

```text
seg_steelbargroup 保存顺序不是按偏移升序。
新设计文件格式必须记录 legacyWriteOrder：
+68 -> +72 -> +80 -> +96 -> +88 -> +64。
read 函数已闭合，但 ref80/ref96/ref88/+64 业务名未闭合，仍需运行保存差异和父子关系对照。
```

### `seg_steelbargroup` 读取函数补证

关键函数：

```text
sub_1405DA5D0
```

包装入口：

```text
sub_1405D5600
```

包装入口行为：

- `ACIS_OBJECT::operator new(112, ..., "e:\tushi3d\dam\class\seg_steelbargroup.cpp")`
- 调用 `sub_1405C6FB0(v0, 0)` 初始化对象。
- 调用 `sub_1405DA5D0(v1)` 读取字段。

已确认读入：

- `a1 + 68` int。
- `a1 + 72` string。
- `a1 + 80` ptr。
- `a1 + 96` ptr。
- `a1 + 88` ptr。
- `a1 + 64` int。

结论：

```text
seg_steelbargroup 的 save/read 顺序成对确认：
+68 -> +72 -> +80 -> +96 -> +88 -> +64。

持久化字段里 +72 是字符串，不是 OCCT/ACIS 几何边。
此前算法链中看到的 entity + 72 更可能属于运行期 payload 或其他对象上下文，
不能直接和 SFL 持久化字段 +72 合并解释。
ref80/ref96/ref88/+64 业务名仍未闭合，需要运行保存差异和父子关系对照。
```

### Detail `StbGeo`

字符串：

```text
StbGeo%d
```

关键函数：

```text
sub_14063B010
```

已确认方向：

- 该函数处理 `StbGeo%d` 写出。
- 字符串包含 `_x / _y / _z / Point%d%s`。
- 函数读取 `EDGE::geometry`、`ELLIPSE::centre`、`major_axis`、`radius_ratio` 等几何信息。

结论：

```text
Detail writer 的 StbGeo 是工程图派生几何。
新设计文件格式必须能从 RebarSegment + OCCT geometryRef 派生 StbGeo。
```

### Detail `StbGroup`

字符串：

```text
StbGroup%d
StbGroups
Std%d
```

关键函数：

```text
sub_14063E910
```

包装 / 上游函数：

```text
sub_14061F830
```

上游行为：

- 创建 `StbGroups` 节点。
- 遍历 `a1 + 528` 链表统计钢筋组数量。
- 写 `stbGroupCount`。
- 对每个组调用 `sub_14063E910`。

`sub_14063E910` 已确认写出：

- 创建 `StbGroup%d` 节点。
- 写 `rsdID`。
- 写 `groupID`。
- 写 `diameter`。
- 写 `diameter2`。
- 写 `interval`。
- 写 `barcount`。
- 写 `segcount`。
- 写 `stbNum`。
- 写 `stbNumAct`。
- 写 `stbLevel`。
- 写 `stbLayer`。
- 写 `stbProfile`。
- 写 `stbUse`。
- 写 `RangeLess180`。
- 写 `ComponentName`。
- 写 `PJSteelName`。
- 写 `SteelWay`。
- 写 `stbType`。
- 写 `stbOffsetInOut`。
- 创建 `Std%d` 子节点。
- 对每段调用 `sub_14063B010` 生成 `StbGeo%d`。
- 对 `Std%d` 写 `segCount`。
- 点状钢筋 `pointStb` 时调用 `sub_14063A860` 生成 `FaceEdge`。

关键分支：

- `rsdID` 不是单一 int 规则：
  - `+104 >= 10000` 时格式化为 `%d%c`。
  - 类型 `+376 == 11` 时格式化为 `Y%d`。
  - 类型 `+376 == 12` 时格式化为 `Z%d`。
  - 某些版本 / 类型分支会取字符串字段。
  - 否则写 int。
- `groupID` 来自旧对象 `+4` 的 `%d` 字符串。
- `barcount` 来自同组链上序号最小 / 最大值差值 `max - min + 1`。
- `segcount` 来自 `sub_1405EE850(*(group + 56))`。
- `stbNum` 来自当前 writer 上下文 `this + 40`。
- `stbNumAct` 来自 `sub_1405BD060(pattern + 43)`。
- `SteelWay` 由部位 / 名称字符串匹配后写 `LA / LASAME / XI / GU / OTHER`。
- `stbType` 按 `pattern[192] == 'C'` 区分 `pointStb` / `lineStb`。

结论：

```text
StbGroup 不是独立事实源。
它由旧钢筋组对象、组内链表、Detail writer 上下文和 StbGeo 子节点派生。

新系统 Detail writer 必须从 RebarModel 派生 StbGroup，
并保证 groupID / rsdID / stbNum / StbGeo.segID / StbTable.rsdID 同源。

rsdID、stbNumAct、SteelWay、stbType、stbOffsetInOut 已有 IDA 写出证据，
但业务生成规则仍需运行样本和 SFL 字段对照闭合。
```

### Detail `StbTable` / `MaterialTable`

字符串：

```text
StbTables
StbTable
count
StbRow%d
rsdID
diameter
stbLevel
length
stbNumSum
lenSum
MaterialTable
rowCount
MatRow%d
singleMass
massSum
Mass
Volume722
```

关键函数链：

```text
sub_140605B20
  -> sub_140602F90
```

辅助 helper：

```text
sub_1405259D0
  -> 按名字查找 XML / COM 子节点

sub_140525880
  -> 按名字匹配子字段并写入 VARIANT 值
```

已确认引用点：

| 字段 / 节点 | 引用点 | 行为 |
|---|---:|---|
| `StbTables` | `0x140603902` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `StbTable` | `0x14060394A` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `count` | `0x1406039A7` | 调用 `sub_140525880` 写值 |
| `StbRow%d` | `0x140603A29` | 使用循环序号格式化行节点名 |
| `rsdID` | `0x140603A7D` | 调用 `sub_140525880` 写值 |
| `diameter` | `0x140603B30`, `0x1406041EF` | 调用 `sub_140525880` 写值 |
| `stbLevel` | `0x140603BE2`, `0x14060413C`, `0x1406044B7` | 调用 `sub_140525880` 写值 |
| `length` | `0x140603C94` | 调用 `sub_140525880` 写值 |
| `stbNumSum` | `0x140603D46` | 调用 `sub_140525880` 写值 |
| `lenSum` | `0x140603DF8`, `0x1406042A1` | 调用 `sub_140525880` 写值 |
| `MaterialTable` | `0x140604008` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `rowCount` | `0x140604068` | 调用 `sub_140525880` 写值 |
| `MatRow%d` | `0x1406040E8` | 使用循环序号格式化行节点名 |
| `singleMass` | `0x140604353` | 调用 `sub_140525880` 写值 |
| `massSum` | `0x140604405` | 调用 `sub_140525880` 写值 |
| `Mass` | `0x1406045E7` | 调用 `sub_140525880` 写值 |
| `Volume722` | `0x140604649` | 调用 `sub_140525880` 写值 |

补充说明：

- `sub_140605B20` 做权限 / 模型状态检查后调用 `sub_140602F90`。
- `MatRow%d` 另在 `sub_1406382D0` 也有引用，本轮只把 `sub_140602F90` 作为下料导出链证据。
- `diameter / stbLevel / lenSum` 在同一函数中存在多个引用点，说明可能覆盖 `StbRow` 与 `MatRow` 或条件分支，不能仅凭本轮地址判断唯一业务来源。

结论：

```text
StbTable / MaterialTable 字段不是样例包偶然字段。
它们在旧程序导出链中由 sub_140602F90 通过 helper 写出。

新系统 Detail writer 必须把 StbTable.rsdID、StbRow 长度 / 数量汇总、
MaterialTable 质量 / 体积汇总与 RebarModel 中的 RebarGroup / ScheduleRow 同源绑定。

字段值公式仍需继续通过 IDA 和旧图石运行样本闭合。
```

### 下料表内部命令绑定

证据 ID：

```text
E-IDA-020
```

已确认字符串：

```text
psexcel
```

字符串地址：

```text
0x140768FF8 -> "psexcel"
```

本轮按真实方向重新读取命令表：

```text
[commandId, string]
```

当前高置信命令映射：

```text
35057 / 0x88F1 -> psexcel
36124 / 0x8D1C -> psallc
36050 / 0x8CD2 -> ysteelout
36059 / 0x8CDB -> yscreenout
36324          -> spoucutt_localcut
36325          -> spoucutt_localexit
```

handler 表仍确认：

```text
psexcel -> sub_140605B20
psallc  -> sub_140600AA0
```

handler 链：

```text
psexcel
  -> sub_140605B20
  -> sub_140602F90
  -> sub_140601D80
```

已确认含义：

- `psexcel` 在内部命令表中应绑定 `35057 / 0x88F1`，不是 `0x8D1C`。
- `psexcel` 在内部 handler 表中绑定 `sub_140605B20`。
- `sub_140605B20` 做权限 / 模型状态检查后，调用 `sub_140602AE0` 查找模板，再调用 `sub_140602F90`。
- `sub_140602F90` 已由 `E-IDA-019` 确认写出 `StbTable / MaterialTable` 字段。
- `sub_140601D80` 引用 `钢筋下料表` 字符串，并属于 `excelcommand.cpp` 导出链。

本轮不能过度推断：

- 尚不能证明顶部 Ribbon 文本 `下料表` 一定直接绑定 `35057 / 0x88F1 / psexcel`。
- 尚不能证明 Dialog #427 一定由 `psexcel` 打开。
- 尚不能证明运行时点击 `下料表` 的输出目录、文件名和覆盖行为。

结论：

```text
内部下料表 / Excel 命令绑定已纠偏为：

35057 / 0x88F1 -> psexcel -> sub_140605B20

但 UI 顶部 caption、Dialog #427 和运行输出仍需旧图石运行确认。
```

### 工程图 / 输出 Ribbon UI 绑定补证

证据 ID：

```text
E-IDA-021
```

关键函数：

```text
sub_1406F37B0
```

当前判断：

- 该函数构造 Codejock Ribbon。
- 反编译中出现 `CXTPRibbonBar`、`CXTPRibbonTab`、`CXTPRibbonGroup`、`CXTPControls::Add`。
- 它创建 `工程图` 页签。
- 它在该页签下创建 `输出` 分组。
- 它向 `输出` 分组加入两个 command id：

```text
36124
35057
```

- 其中 `36124` 还会被加入 QuickAccess 控件。
- `35057` 当前没有在同一处 QuickAccess immediate 列表命中。

相关字符串：

```text
0x140796BBC -> 工程图
0x1407969D4 -> 输出
0x14077C328 -> 钢筋下料表
0x14077C32C -> 下料表
```

和 `E-IDA-020` 的关系：

```text
E-IDA-020:
  35057 / 0x88F1 -> psexcel -> sub_140605B20

E-IDA-021:
  工程图 / 输出 Ribbon 分组 -> {36124, 35057}
```

已确认含义：

- `工程图 / 输出` 下不是单一命令，而是至少两个旧命令入口。
- 结合命令表纠偏后，`35057` 才是当前高置信下料表 / Excel 候选入口。
- `36124` 不能再直接写成下料表按钮。

本轮不能过度推断：

- 还不能单靠本证据证明按钮最终显示标题一定是 `下料表`，需要继续确认命令 caption / resource 绑定。
- 还不能证明 Dialog #427 一定由 `35057 / psexcel` 打开。
- 还不能证明运行时输出目录、文件名、覆盖策略。

结论：

```text
工程图 / 输出 Ribbon 分组
  -> {36124, 35057}

其中：
  35057 当前与 psexcel / 下料表链对应
  36124 保留给生成工程图侧继续追证
```

### 生成工程图命令入口 / Ribbon 绑定静态补证

证据 ID：

```text
E-IDA-042
```

本轮复核时重开后的 IDA MCP 会话：

```text
session = visualts_todo062_generate_drawing
database = VisualTS.exe.i64
Hex-Rays = ready
```

关键函数：

```text
sub_1406F37B0
sub_140600AA0
```

`sub_1406F37B0` 当前判断：

- 该函数就是旧 `VisualTS` 的 Codejock Ribbon 构造函数。
- 它创建 `工程图` 页签和 `输出` 分组。
- 它在这个分组里直接加入两个 command id：

```text
36124
35057
```

- 其中 `36124` 还会被加入 QuickAccess。

相关反编译片段可收口为：

```text
v75 = CXTPRibbonBar::AddTab(v8, byte_140796BBC);            -> 工程图
v82 = CXTPRibbonTab::AddGroup(v75, dword_1407969D4);        -> 输出
CXTPRibbonGroup::Add(v82, 1, 36124, ...)
CXTPRibbonGroup::Add(v82, 1, 35057, ...)
```

结合命令表真实方向，本轮应纠偏为：

```text
36124 / 0x8D1C -> psallc
35057 / 0x88F1 -> psexcel
36050 / 0x8CD2 -> ysteelout
36059 / 0x8CDB -> yscreenout
```

`sub_140600AA0` 当前判断：

- 该函数内部先过 `sub_1404DEA40(5)` 许可门。
- 它创建 `StbTables` 和 `HViewPorts / ViewPort / PartDetailDrawing / StbDetailDrawing`。
- 它会调用：

```text
sub_140609690   -> StbTable / MaterialTable
sub_140609A50   -> 一类对象视图生成
sub_14060B010   -> 一类对象视图生成
sub_14060A230   -> 一类对象视图生成
sub_14060DC00   -> 一类对象视图生成
sub_14061F970   -> PartDetailDrawing 复杂线容器
sub_14061F830   -> StbGroups / StbGroup / Std / StbGeo
sub_140635A80   -> 保存 DetailNN.stl
```

当前高置信静态收口：

```text
顶层 Ribbon 层：
  工程图 / 输出 -> {36124, 35057}

生成工程图 / Detail 包主写出链：
  36124 / 0x8D1C -> psallc -> sub_140600AA0
    -> Detail.xml / DetailNN.stl

下料表 / Excel 链：
  35057 / 0x88F1 -> psexcel -> sub_140605B20
    -> sub_140602F90 -> StbTable / MaterialTable

额外纠偏：
  0x8CD2 -> ysteelout
  不是 psallc
```

本轮不能过度推断：

- 还不能只凭静态 immediate 直接写死 `36124` 的中文 caption 一定就是 `生成工程图`。
- 还不能确认 `35057` 和 `36124` 的中文 caption / resource 最终归属。
- 还不能证明运行时点击 `生成工程图` 的输出目录、覆盖策略和旧插件导入结果。

结论：

```text
TODO-062 / TODO-063 合并后的当前 static stop point：

工程图 / 输出
  -> 36124 / 0x8D1C -> psallc -> sub_140600AA0
  -> 35057 / 0x88F1 -> psexcel -> sub_140605B20

其中：
  36124 当前更像生成工程图 / Detail 包入口
  35057 当前更像下料表 / Excel 入口
```

### 生成工程图 / 下料表命令映射纠偏与 Dialog #274 降级

证据 ID：

```text
E-IDA-043
```

关键函数：

```text
sub_1406AD840
sub_1406AC4A0
sub_1406AD9D0
```

当前判断：

- `sub_1406AD840` 会调用 `CPropertyPage::CPropertyPage(..., 0x112, ...)`。
- `0x112` 就是 `Dialog #274`。
- 该函数直接把 vftable 设到：

```text
Set2Dpage
```

- 它的 callers 位于一个属性页集合里，该集合同时挂了：

```text
Set2Dpage
Set3Dpage
proofwater
OptionDisplay
OptionFactory
CsetYJK
```

已确认含义：

- `Dialog #274` 当前只能保守记为 `Set2Dpage` 属性页。
- 它不再能直接写成“生成工程图设置窗口”。

当前剩余缺口：

```text
1. 36124 / 35057 的中文 caption / resource 最终归属
2. 真正“生成工程图”弹窗候选及打开链
3. 运行时输出目录、覆盖策略和旧插件导入结果
```

## TODO-064 中文 caption / resource 绑定与真正生成工程图弹窗候选补证

证据 ID：

```text
E-IDA-044
```

本轮复用会话：

```text
database = visualts_todo062_generate_drawing
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

### 1. `36124 / 35057` 的中文 caption 已可静态闭合

本轮额外用 `VisualTS.exe` 做了 Win32 `LoadStringW` 资源探针。

直接读到：

```text
ID=36124 -> 用定义的剖面剖切实体模型\n生成工程图
ID=35057 -> 生成下料表Excel文件\n下料表
ID=36050 -> 输出钢筋几何\n输出钢筋
```

当前高置信工程口径：

```text
旧资源字符串采用：
  描述/help \n caption

所以顶部命令 caption 当前可静态闭合为：
  36124 -> 生成工程图
  35057 -> 下料表

36050 是同一模式的直接旁证：
  输出钢筋几何 \n 输出钢筋
```

### 2. 真正生成工程图弹窗候选进一步收窄

本轮继续复核：

```text
sub_140600AA0
sub_140601600
sub_1404FE300
```

当前高置信链：

```text
36124 / 0x8D1C
  -> psallc
  -> sub_140600AA0
  -> sub_140601600(v64, &v83)
  -> if (v83) {
       sub_1404FE300(v81, 0)
       CDialog::DoModal(v81)
     }
```

进一步收窄：

```text
sub_140601600
  -> GetTempPathA(...)
  -> 拼出 UnCutSteel.TXT
  -> 写未割钢筋 / 未切净类文本
  -> 命中时把 *a2 = 1

sub_1404FE300
  -> CDialog::CDialog(..., 0x57C, ...)
  -> vftable = output_uncut_steel
```

因此当前最稳妥结论是：

```text
sub_140600AA0 里能直接证明的 DoModal，
不是前置“生成工程图设置窗”，
而是：
  output_uncut_steel
  / Dialog 0x57C
  / UnCutSteel.TXT

它更像生成后的未割钢筋报告 / 提示窗候选。
```

### 3. `Dialog #427` 继续降级，不能再拿来证明“下料表弹窗”

本轮复核：

```text
sub_1406AC240
  -> CPropertyPage(..., 0x1AB, ...)
  -> 0x1AB = 427
  -> vftable = OptionFactory
```

同时：

```text
sub_1406AD120
  -> sub_1406AC4A0(..., byte_1407898F8, ...)
  -> CPropertySheet::DoModal(...)

sub_1406AC4A0
  -> COptionSheet
  -> Add(Set2Dpage / Set3Dpage / proofwater / OptionDisplay /
         OptionFactory / CsetYJK ...)
```

所以当前只能保守写成：

```text
Dialog #427
  -> OptionFactory 属性页
  -> 属于系统设置属性页家族

它不再能作为
  35057 / 下料表
的直接弹窗真值。
```

### TODO-064 收口后的静态 stop point

```text
caption / resource：
  36124 -> 生成工程图
  35057 -> 下料表

真正生成工程图弹窗候选：
  当前只收窄到
    output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
  这条后置报告链

不能再写成：
  Dialog #274 = 生成工程图设置窗
  Dialog #427 = 下料表弹窗
```

### 本轮仍不证明

```text
旧图石点击 生成工程图 时的真实输出目录
Detail.xml + DetailNN.stl 的真实覆盖策略
旧图石点击 下料表 时是否还会出现额外运行时弹窗
旧插件 AutoCAD L2 已通过
golden 已闭合
```

### 下料表 Detail / XML writer 聚合链补证

证据 ID：

```text
E-IDA-025
```

本轮复核时重开后的 IDA MCP 会话：

```text
session = visualts_i64_todo030
database = VisualTS.exe.i64
Hex-Rays = ready
```

已确认调用链：

```text
sub_140609690
  -> sub_140606560
  -> sub_140609860
  -> sub_140607E30
       写 StbTable
       循环调用 sub_140637400 写 StbRow%d
  -> sub_140609190
       写 MaterialTable
       循环调用 sub_1406382D0 写 MatRow%d
```

关键字段来源：

```text
sub_140607E30:
  StbTable.count = vector size
  每行调用 sub_140637400(row, ...)

sub_140637400:
  rsdID = Y%d / Z%d / %d%c / string / int 多分支
  ComponentName = 旧构件链字段
  SteelWay = GU / LA / STEEL_NET / LASAME / XI / OTHER 分支
  diameter = a1[8]
  length = sub_140637070(...)
  segNum = sub_1405FE280(v5)
  sameGrpNum = a1[1]
  stbNumSum = a1[3]
  lenSum = *((double*)a1 + 5)，格式 %.2f
  stbLevel = *(v5 + 56 + 136)

sub_140609190:
  MaterialTable.rowCount = material vector size
  循环调用 sub_1406382D0(row, ...)
  MaterialTable.Mass = sum(sub_140637250(row)) / 1000
  MaterialTable.Volume722 = ACIS body mass_props volume，四舍五入到 0.001
  MaterialTable.MassNum = 入口 a1

sub_1406382D0:
  diameter = sub_140637010(this) -> *(this + 4)
  lenSum = sub_140637220(this) -> *(this + 8)
  countSum = *(this + 0)
  singleMass = CAnimationVariable::GetDefaultValue(this)
  massSum = sub_140637250(this) -> a1[2] * a1[1]
  stbLevel = sub_140637200(this) -> CString(this + 32)
```

和 `E-IDA-019 / E-IDA-042 / E-IDA-043` 的边界：

```text
E-IDA-019 确认 sub_140602F90 是旧 Excel / 下料表字段写值链。

E-IDA-042 / E-IDA-043 后续又把命令映射纠偏为：
  35057 / 0x88F1 -> psexcel -> sub_140605B20 -> sub_140602F90
同时确认：
  36124 / 0x8D1C -> psallc -> sub_140600AA0
是 Detail.xml + DetailNN.stl 主写出链。

E-IDA-025 确认 Detail / XML writer 侧的 StbTable / MaterialTable 聚合写出链。
两者字段重叠，但不能把函数来源混成同一条业务链。
```

本轮不能过度推断：

```text
sameGrpNum 的旧合并规则仍需继续追 sub_140606560 / sub_140609860。
singleMass 的旧材料表来源仍需继续追 material row 构造逻辑。
Volume722 依赖旧 ACIS body mass_props；新系统 P0 不声明等价。
```

### 工程图 DetailNN 多图纸命名补证

证据 ID：

```text
E-IDA-026
```

本轮复核时重开后的 IDA MCP 会话：

```text
session = visualts_i64_todo031_recheck
database = VisualTS.exe.i64
Hex-Rays = ready
```

已确认保存函数：

```text
sub_140635A80(a1, viewIndex)
```

关键反编译结论：

```text
GetTempPathA(...)
  -> 追加 "msohtmplcllip"

if viewIndex < 10:
  format "\\Detail0%d.stl"
else:
  format "\\Detail%d.stl"
```

含义：

```text
Detail01.stl ... Detail09.stl 使用两位补零。
Detail10.stl 以后使用自然数字。
Detail100.stl 不是 Detail0100.stl。
```

与工程图主链关系：

```text
sub_140600AA0
  -> 创建 StbTables / HViewPorts / ViewPort / PartDetailDrawing / StbDetailDrawing
  -> 调用 sub_140609690 写 StbTable / MaterialTable
  -> 调用 sub_14061F830 写 StbGroups
  -> 调用 sub_140635A80(ppv, viewIndex) 保存 DetailNN.stl
```

本轮不能过度推断：

```text
E-IDA-026 只确认 DetailNN.stl 命名和保存链。
不证明剖切线、隐藏线、填充线、标注或 AutoCAD L2 已完成。
旧图石运行输出目录和旧插件是否接受 Detail100.stl 仍需运行确认。
```

## 右键命令证据摘要

来自已有迁移资料 `ROOT_CONTEXT_MENU_COMMANDS.csv`：

| 旧菜单行为 | CommandId | 新系统目标 |
|---|---:|---|
| 激活/钝化 | `0x8CA0` | `RebarGroup.ToggleActive` |
| 钢筋组合并 | `0x891F` | `RebarGroup.MergeGroups` |
| 钢筋段连接 | `0x8924` | `RebarGroup.ConnectSegments` |
| 特性 | `0x8933` | `RebarGroup.OpenProperties` |
| 删除 | `0xE120` | `RebarGroup.Delete` |
| 增减根数 | `0x8CEB` | `RebarGroup.EditBarCount` |
| 修改间距 | `0x8CE6` | `RebarGroup.EditSpacing` |
| 复制 | `0x8CE7` | `RebarGroup.Copy` |
| 移动 | `0x8CEC` | `RebarGroup.Move` |
| 与线裁剪 | `0x8939` | `RebarGroup.TrimByLine` |
| 与面裁剪 | `0x8CE5` | `RebarGroup.TrimByFace` |
| 设为箍筋 | `0x8D4A` | `RebarGroup.SetAsStirrup` |

## TODO-020 线筋 / 弧筋链 IDA MCP 补证

Evidence ID：

- `E-IDA-022`

本轮 IDA MCP 成功打开：

```text
VisualTS.exe.i64
session_id = visualts_i64_todo020
hexrays_ready = true
```

本轮确认链路：

```text
sgroupbarline -> sub_1404DE720 -> sub_1404D10C0
sgroupbararc  -> sub_1404DE110 -> sub_1404D10C0
sub_1404D10C0 -> sub_140451730 -> sub_1405D5670
sub_1405D5670 -> sub_1405BD0C0
sub_1404D10C0 -> sub_1405C7260 -> sub_1405E49D0
```

`sub_1404DE720` 本轮新增确认：

- 选择集必须只有 1 个对象。
- 对象必须通过 `sub_1405C6820` 和 `sub_1405DA020` 校验。
- 内部子项数量必须不少于 2。
- 从奇数索引子项抽取实体并组装临时 `ENTITY_LIST`。
- 读取两条相关边的起止点，计算 4 种端点距离。
- 初始最小距离候选为 `10.0`。
- 调用：

```text
sub_1404D10C0(copiedEntityList, v6, v4, minDistance, selectedEndpointDistance, flag)
```

`sub_1404DE110` 本轮新增确认：

- 选择集必须只有 1 个对象。
- 对象必须通过 `sub_1405C6820` 和 `sub_1405F17C0` 校验。
- 内部元素数量必须不少于 3。
- 从链式实体关系收集临时 `ENTITY_LIST`。
- 默认距离为 `0.1`。
- 调用 ACIS：

```text
api_entity_entity_distance(entityA, entityB, pointA, pointB, &distance, ...)
```

- 调用：

```text
sub_1404D10C0(copiedEntityList, v4, v6, distance, 0.8, 1)
```

`sub_1404D10C0` 本轮新增确认：

- `objA / objB` 不能为空。
- `sub_1405F25F0(objA) >= 3`。
- `ENTITY_LIST::count(entityList) >= 1`。
- `distanceA >= 0.002`。
- 进入 `api_bb_begin(1)` ACIS block。
- 创建成功后依次执行几何修正、父组刷新、显示挂接、dirty 标记。

`sub_1405D5670` 本轮新增确认：

- 命中源码路径 `e:\tushi3d\dam\class\seg_steelbargroup.cpp`。
- 通过 `api_edge(entity + 72, &edge)` 获取 ACIS `EDGE`。
- 用 `api_entity_entity_distance` 取得 edge 到参考实体的最近点。
- 最近点不是端点时调用 `api_split_curve(edge, pointOnEdge, ...)`。
- split 后小段阈值为 `0.01`。
- 近端判断阈值为 `0.1`。
- spline 重建采样数为 `max(5, EDGE::length(edge) * 50)`。
- 端部 trim 使用 `sub_140580950(&edge, -0.03, 0/1)`。
- 端点保护分支使用 `0.001 / 0.999` 两个比例 split。
- 端部最小距离循环使用 `(EDGE::length(edge) - 0.1) / 0.02` 估算迭代次数。
- 最终用 `sub_1405BD0C0(entity, edge)` 写回。

辅助函数补证：

```text
sub_14058F160(edge, ratio)
  -> ratio 接近 0 取起点，接近 1 取终点，中间走 bounded_curve。

sub_14059B980(group, point)
  -> 遍历 group + 88 链，用 api_entity_point_distance 求组内最小距离。

sub_1405BD0C0(entity, edge)
  -> ENTITY::backup(entity)，然后 *(entity + 72) = edge。

sub_1405E49D0(object)
  -> 在对象树显示名开头插入 *，并调用 sub_1405F2710(object, 1)。
```

本轮不能过度推断：

- `sub_1405D5670` 第 4 个 double 参数在函数内被用作端部最小距离阈值，但 `sub_1404D10C0` 里的反编译调用只显示 3 个显式实参，真实来源仍需继续确认。
- `objA / objB / createdPayload` 的业务对象名仍未完全闭合。
- `sgroupbararc` 对应旧 UI 的 `扇形筋`、`同心圆`，还是二者共用，仍需旧图石运行确认。

## TODO-034 FDrawing 插件工程图对象符号静态证据

- `E-IDA-027`

本轮 IDA MCP 会话：

```text
database = fdrawing_arx_todo034
input = C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawing.arx
```

当前确认：

- IDA MCP `idb_list` 显示 `fdrawing_arx_todo034` 为 active worker session。
- `find_regex` 可命中旧 AutoCAD 插件侧工程图对象 / 方法符号：
  - `CViewInfo`
  - `CWSNLineDim`
  - `CWSNPointDim`
  - `CWSNSteelBarTable`
  - `CWSNMaterialTable`
  - `CWSNSectionTitle`
- `CViewInfo` 可见 `GetBoxXmlMinMax / GetTopDir / GetCutPlanePos / GetCutPlaneDir / GetCutPlaneDir0 / GetGeneralScale / GetDimension*` 等视图和剖切相关方法符号。
- `CWSNLineDim / CWSNPointDim` 可见钢筋标注、位置、bulge、explode 等插件对象方法符号。

本轮不能过度推断：

- 在 `FDrawing.arx` 字符串缓存里暂未把 `continue-line / hidden-line / section-line / hatch-line / steeljoint-line / FaceEdge / StbGroup / StbGeo / ZValue` 等 XML 节点名作为明文命中。
- 因此复杂 XML 字段名的直接证据仍来自旧 `Detail01.stl` 样例，`FDrawing.arx` 只作为旧插件对象模型存在的静态补强证据。
- 旧插件是否接受缺失 / 空复杂字段容器，仍需 AutoCAD L2 导入或进一步反编译确认。

## TODO-045 真实接头线 / Others 生成规则 IDA MCP 补证

Evidence ID：

- `E-IDA-028`

本轮复核时重开后的 IDA MCP 会话：

```text
database = visualts_i64_todo045
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
Hex-Rays = ready
```

### 命令入口和旧业务对象

字符串和命令表可确认以下接头命令：

```text
barjointnew   -> sub_1405DFAA0
barjointclear -> sub_1405DF710
barjointmove  -> sub_1405DFEF0
barjointrev   -> sub_1405E02D0
groupjointnew -> sub_1405F0060
groupjointclear -> sub_1405EFCC0
featjointnew / goujianjointnew -> sub_1405EF140 系列
```

这些函数共同特征：

- 先从当前选择集取对象。
- 通过 `sub_1405C6820`、`selected + 13` 或 `sub_1405E0E70` 做旧钢筋 / 构件对象筛选。
- 进入 `api_bb_begin / update_from_bb / api_bb_end` ACIS transaction。
- 调用 `sub_1405DB6C0 / sub_1405DC870 / sub_1405E1D20 / sub_1405E1CC0` 等旧 VisualTS steeljoint 业务函数。

结论：

```text
接头对象由旧 VisualTS steeljoint / steelbar 业务层创建和维护。
ACIS 参与几何和事务。
Detail writer 只在出图阶段消费旧对象 / 渲染上下文，不是凭空生成接头业务。
```

### steeljoint-line / joints 写出链

`sub_14061F970` 是 `PartDetailDrawing` 复杂线容器 writer。

其中接头线相关链路为：

```text
sub_14061F970
  -> 查找或创建 "steeljoint-line"
  -> 查找或创建子节点 "joints"
  -> 遍历 *(v8 + 528) 的 ha_rendering_context 链
  -> sub_1406107F0(v8, ..., renderingContext, transform, xmlDoc, jointsNode, v8 + 1048)
  -> sub_1405398F0(xmlDoc, jointsNode, start/end positions, counter)
```

`sub_1406107F0` 确认：

- 参数 `a5` 为 `ha_rendering_context *`。
- 调用 `ha_rendering_context::GetPattern(a5)`。
- 遍历 pattern 链：`pattern = *(pattern + 56)`。
- 只处理 `*(int16 *)(pattern + 194) >= 1` 且 `*(byte *)(pattern + 192) == 0x4C ('L')` 的 pattern。
- 从 `pattern + 344 -> +128` 取得 joint 点链。
- 每个 joint 点从 `APOINT::coords(*(pointNode + 64))` 取坐标。
- 从关联 `EDGE` 起点 / 终点求方向并 `normalise`。
- 当前对称半长公式为 `dword_14095D62C / 2000.0`，不是固定常量样例值。
- `sub_1406ED5E0 / sub_1406EDF80 / sub_1406AC410 / sub_1406AC6A0` 已闭合：
  - `JointWeldLength <-> dword_14095D62C`
  - 保存 / 读取口径为 `dword_14095D62C = dialogValue * 10`
  - 所以 writer 使用的对称半长 = `dialogValue / 200.0` 模型单位
  - 整条 `LineN` 总长 = `dialogValue / 100.0` 模型单位
- `sub_1407306A0` 旧 HOOPS 显示链也使用 `(double)dword_14095D62C / 2000.0`，可作为独立旁证。
- 对每个 joint 点生成沿 edge 方向的前后两端点，并在存在 transform 时变换。
- 调用 `sub_1405398F0` 写入 `joints` 下的 `LineN`。

`sub_1405398F0` 确认 `LineN` 字段：

```text
Line%ld
  start_x
  start_y
  end_x
  end_y
  ZValue
```

`ZValue` 格式：

```text
"%f:%f:%f"
  -> z_start_or_projected
  -> z_end_or_projected
  -> distance_to_point(start, end)
```

其中 `sub_1405398F0` 会在存在视图/剖切上下文时，用 `sub_14054C6F0()` 返回的平面/方向数据修正 Z 值；因此当前不能只按世界坐标 `z` 简化实现完整算法。

### Others / symbolcutIOS 写出链

`sub_14061F970` 中 `Others` 相关链路为：

```text
sub_14061F970
  -> 查找或创建 "Others"
  -> 如果 *(v8 + 848) 存在且当前不是外部传入 transform 分支
  -> 创建 "symbolcutIOS" 子节点
  -> 遍历 *(v8 + 840) 环形链
  -> sub_14053A3F0(xmlDoc, symbolcutIOSNode, transformedCenter, code, counter)
```

`sub_14053A3F0` 确认 `SymbolCutIOSN` 字段：

```text
SymbolCutIOS%ld
  center_x
  center_y
  center_z
  code
```

结论：

```text
Others 在旧样例中为空，不代表旧系统永远为空。
旧 writer 存在 symbolcutIOS / SymbolCutIOSN 写出路径。
其输入来自 v8 + 840 / v8 + 848 的切割符号链，而不是普通线容器。
```

### 额外接头弧线分支

`sub_14061F970` 在 `sub_1406107F0` 之后还有一个分支：

```text
if (!sub_140610AA0(v8)) {
  遍历 ha_rendering_context pattern
  取 joint / segment 相关 edge
  proj_pt_to_plane
  api_curve_arc_center_edge
  sub_14053A6B0(...)
  ++*(v8 + 1052)
}
```

`sub_140610AA0(v8)` 只是读取 `*(uint16 *)(v8 + 232)`，它是视图 / 出图上下文 flags。

结论：

```text
旧接头线不只有 joints/LineN 这一种输出。
在特定 flags 下，还可能额外生成 arc/line 类几何并走 sub_14053A6B0。
```

### TODO-046 参数绑定与运行触发补证

本轮继续追 `JointRuler / JointDistbet / JointWeldLength`、`pattern + 192`、
额外 `api_curve_arc_center_edge` 分支和 `Others / symbolcutIOS` 触发链，当前可确认：

- `sub_1406ED5E0 / sub_1406EDF80 / sub_1406AC410 / sub_1406AC6A0` 已把三个旧参数闭合为：

```text
JointRuler      <-> dword_140994AB8
JointDistbet    <-> dword_14095D628
JointWeldLength <-> dword_14095D62C
```

- `sub_1405DB340`（`__SteelBar` 构造）会把 `dword_140994AB8`
  直接写到对象字段 `+108 / +112`。
- `sub_1405E9640` 和 `sub_1405DFAA0(barjointnew)` 会用
  `JointRuler` 作为基础长度，并在偶数 / 奇数位上用
  `JointDistbet` 做错位修正：

```text
odd/even parity
  -> end = JointRuler
  -> or end = JointRuler - JointDistbet
```

- `sub_1405EBA30` 延续同样的 `JointDistbet` 纠偏规则。
- `sub_1405DFEF0(barjointmove)` 会把对象字段 `+112` 归一化到
  `field112 mod field108`，说明 `JointRuler` 和当前相位 / 偏移字段
  已落到旧对象内部，而不是只停留在对话框全局值。

- `pattern + 192 == 76` 这一点，现在至少能从汇编确认成：

```text
cmp byte ptr [pattern + 0xC0], 0x4C
  -> raw discriminant is literal 'L'
```

这比“裸 76”更明确，但拥有该字段的结构名 / 枚举名仍未闭合。

- `Others / symbolcutIOS` 当前 gate 已进一步明确：

```text
if (*(v8 + 848) && a4 == 0)
  -> create Others/symbolcutIOS
  -> iterate ring list *(v8 + 840)
```

其中单个 list node 至少可确认包含：

```text
node + 16  -> int code
node + 24  -> SPAposition center
```

写出前会先经过当前 `SPAtransf` 变换，再调用 `sub_14053A3F0`。

- `sub_14061F970` 额外 `api_curve_arc_center_edge` 分支当前已能确认更细的触发条件：

```text
sub_140610AA0(v8) == 0
  -> flags word at (context + 232) is zero
pattern[43] != 0
distance_to_plane(firstPoint, viewPlane) < 0.4
visibility / direction gate passes
  -> based on *(v8 + 728 / 732) and dot(viewDir, candidateDir)
```

一旦进入该分支：

```text
*(byte *)(v8 + 832) = 1
api_curve_arc_center_edge(...)
sub_14053A6B0(...)
++*(v8 + 1052)
```

随后函数尾部会把 `General-Info/DrawTaoTong` 写成 `T`。
因此当前至少可以确认：额外弧线分支和 `DrawTaoTong` 置位存在直接关系，
不是普通 `joints/LineN` writer 单独决定的。

本轮不能过度推断：

- `JointWeldLength` 的数学绑定已闭合，但旧 UI 显示单位名仍需运行确认。
- `pattern + 192` 的 raw byte `'L'` 已确认，但 owning enum / 结构名未确认。
- `pattern + 344 / +128 / pointNode + 64 / pointNode + 88` 的结构名未完全闭合。
- `sub_14061F970` 额外 `api_curve_arc_center_edge` 分支的容器业务名和旧 UI 关系未闭合。
- `sub_1406BA2C0` 在特定文件元数据包含 `Varies_ShangH_LZ` 时会把 `dword_14095D62C` 改写为 `800`；这是否属于旧项目默认值或专项导入规则仍需运行确认。
- `*(v8 + 840 / 848)` 的 producer 和旧 UI 触发路径未闭合。
- 旧 AutoCAD FDrawing 插件是否接受 TODO-043/044 的空容器包仍需 L2 运行确认。
- 本轮只补证据，不实现真实接头线 / Others 几何算法。

## TODO-048 旧图石启动阻塞提示链补证

Evidence ID：

- `E-IDA-030`

本轮复核使用的 IDA MCP 会话：

```text
database = visualts_i64_todo045
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
Hex-Rays = ready
```

### 启动阻塞主链

当前可确认旧图石启动期阻塞链为：

```text
sub_1406BBFC0
  -> sub_1406BC3B0
  -> sub_14070C760(&qword_140994BF8, a1szphyqnlqkepf, 0)
```

`sub_1406BBFC0` 当前确认：

- `AfxOleInit()` 成功后进入旧图石启动主流程。
- 如果 `sub_1406BC3B0() > 0`：

```text
dword_140994BF0 = 2
appFlag(+178) = 1
continue -> sub_1406FF100(this)
```

- 如果 `dword_140994BF0 == -1`：

```text
appFlag(+178) = 0
return 0
```

- 如果 `dword_140994BF0 == 0`：

```text
appFlag(+178) = 0
CWnd::MessageBoxA(..., "请检查网线是否接好", "提示", MB_ICONHAND)
return 0
```

工程含义：

```text
旧图石启动不是“点开就进主界面”。
它先过一层旧许可 / 网络相关检查。
失败时先弹阻塞框，再直接终止进入主界面的路径。
```

### 错误码 41 和提示文案映射

`sub_1406BC3B0` 当前确认：

```text
v0 = sub_14070C760(&qword_140994BF8, a1szphyqnlqkepf, 0)
if (v0 == 41) {
  dword_140994BF0 = -1
  MessageBoxA(..., "许可已过期", "提示", MB_ICONHAND)
  return 0
}
return !v0
```

可开发级结论：

```text
v0 == 0
  -> 启动检查通过

v0 == 41
  -> 明确映射到“许可已过期”

其他非 0 值
  -> 启动主链会落到“请检查网线是否接好”
```

这说明当前用户看到的 `提示 / 请检查网线是否接好`，
不是一个泛化的随机文案，而是旧启动检查对“非 0 且非 41”
失败码的统一 fallback。

### 文案字符串闭合

IDA 反编译里能直接看到三个字节地址：

```text
byte_14075A738  -> MessageBox title
byte_14078D7A8  -> expired text
byte_14078D820  -> fallback text
```

本轮同时用 `VisualTS.exe` 原始二进制按 GB2312 解码复核了对应 raw offset：

```text
0x758F38 -> 提示
0x78BFA8 -> 许可已过期
0x78C020 -> 请检查网线是否接好
```

并且 data-ref 已和启动链闭合：

```text
0x14078D820 -> 0x1406bc094  -> sub_1406BBFC0 fallback MessageBoxA
0x14078D7A8 -> 0x1406bc3fc  -> sub_1406BC3B0 expired MessageBoxA
0x14075A738 -> 多个 MessageBox title 引用，其中包含上述两处启动链
```

### 许可栈侧证据

`sub_14070C428` 当前确认：

```text
*a1 = &ChaspBase::vftable
```

这说明 `sub_14070C760` 所在检查链至少和一类 `ChaspBase`
许可对象相关。

本轮 `find_regex` 继续命中的侧证据包括：

```text
NETHASP_00112233445566zz
HASP-HL
HASP-SL
HASP-SL-AdminMode
HASP-SL-UserMode
SuperDog
sentinelhl,hasphl
hasp_enabled
nethasptype
HL_LICENSEDIR
hlrus_license_file.alf
```

对应 xref 当前可确认：

```text
NETHASP_00112233445566zz -> sub_140237E8A
nethasptype             -> sub_14018B7D0
hasp_enabled            -> sub_1401882A8
HL_LICENSEDIR           -> sub_14031C11B
hlrus_license_file.alf  -> sub_14031C11B
```

工程含义：

```text
旧图石启动阻塞高度相关于 Sentinel / HASP / SuperDog / NetHASP 一类许可栈。
HL_LICENSEDIR / hlrus_license_file.alf 也是同一许可生态中的文件侧证据。
```

但本轮不能过度推断：

- `HL_LICENSEDIR / hlrus_license_file.alf` 当前只作为侧证据，不能写成“本机当前一定就是这个文件缺失”。
- 当前仍不能单靠 IDA 静态证据判断用户这台机器是本地加密狗、网络许可，还是其他同栈模式。

### 共用许可 gate，不只启动时触发

`sub_1404DEA40` 当前确认也会复用：

```text
sub_14070C760
sub_14070C428
sub_14070C7E0
```

其 caller 当前可见包含：

```text
sub_14055C1F0
sub_140600AA0
sub_140605B20
...
```

结论：

```text
这不是只在程序启动时用一次的孤立弹窗链。
它还是旧图石多处钢筋 / 工程图 / 输出功能共用的许可 gate。
所以当前启动先被挡住，后面也不应该假设接头命令、工程图命令一定能直接跑。
```

### 用户手工解除前置条件清单

基于当前静态证据，后续用户手工排查建议按以下优先级执行：

```text
1. 先确认旧许可本身没有过期。
2. 确认 Sentinel / HASP / SuperDog 相关许可环境或服务已就绪。
3. 如果是网络许可，先确认本机网络可用。
4. 如果是网络许可，确认 license server / 网络狗宿主机可达。
5. 若仍失败，再检查 HL_LICENSEDIR 和 hlrus_license_file.alf 相关文件链。
```

置信度边界：

- 1~4：高。
- 5：中；当前属于许可栈侧证据，不是已闭合的主因。

## TODO-049 启动前置条件 IDA 补充

Evidence ID：

- `E-IDA-031`

本轮复核时重开的 IDA MCP 会话：

```text
database = visualts_i64_todo049
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
Hex-Rays = ready
```

### fallback 不是纯“网络报错”

`sub_14070C760` 当前可确认：

```text
sub_14070CBF0(&v9)
if (licenseContextReady && sub_14070D178(&v9, a1 + 8) != 0)
  return sub_14070D454(...)
else
  return 500
```

其中：

```text
sub_14070CBF0
  -> 先取线程级许可上下文
  -> 再用 sub_14070D618(v2 + 24) 判断上下文是否 ready

sub_14070D618
  -> *(byte *)(ctx + 40) == 0 时直接返回 0
  -> 否则 EnterCriticalSection(ctx) 后返回 1

sub_14070D178
  -> 如果 ready 标志为 0，直接返回 0
  -> ready 为 1 时，再按映射表取具体许可对象
```

工程含义：

```text
“请检查网线是否接好” 这条旧文案并不等于
“已经明确走到网络许可握手失败”。

只要许可上下文没 ready、对象映射失败，
sub_14070C760 就会先返回 fallback 路径。
```

所以当前更准确的表述应该是：

```text
这是一条宽兜底的许可 / 环境初始化失败文案，
不是只能解释成“网线没插”。
```

### 许可字符串更像底层栈标记，不是 app 级直接配置口

本轮复核字符串 xref：

```text
HL_LICENSEDIR             -> sub_14031C11B
hlrus_license_file.alf   -> sub_14031C11B
hasp_enabled              -> sub_1401882A8
nethasptype               -> sub_14018B7D0
NETHASP_00112233445566zz  -> sub_140237E8A
```

当前可保守确认：

```text
这些字符串都落在 Sentinel / HASP / NetHASP 许可栈内部函数里，
当前没有直接落到旧图石启动 wrapper 自己的 app 级配置分支。
```

这说明 `TODO-049` 的人工动作优先级应该保持为：

```text
1. 先确认许可模式：USB 狗 / 网络许可 / 其他。
2. 再确认服务、内网 / VPN、宿主机可达性。
3. 最后再查 HL_LICENSEDIR / license file 目录链。
```

本轮不能过度推断：

```text
- 不能据此写成“当前一定是 network license”。
- 不能据此写成“当前一定是 license file 缺失”。
- 不能据此写成“当前 fallback = 500 就是唯一失败码”。
- 不能据此替代用户现场手工启动结果。
```

## TODO-050 旧图石非空接头线 / Others 运行样例采集准备补证

Evidence ID：

- `E-IDA-032`

本轮重新打开 IDA MCP：

```text
database = visualts_i64_todo050
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
strings_cache_size = 16320
```

本轮目标不是继续实现接头线算法，
而是在旧图石运行样例暂时依赖用户现场操作的情况下，
先补强旧内部命令和触发线索。

### 接头内部命令清单

字符串缓存确认以下旧内部命令：

```text
barjointnew
barjointclear
barjointmove
barjointrev
groupjointnew
groupjointclear
goujianjointnew
goujianjointclear
featjointnew
featjointclear
groupjointrev
groupjointmove
```

`barjointnew` 字符串 xref 落到两组表：

```text
0x140959328
0x14095ae70
```

`groupjointrev` 字符串 xref 同样落到两组表：

```text
0x1409593e8
0x14095af90
```

当前未从这些表项继续追到中文 Ribbon caption。

### 三类接头入口不要混用

`sub_1405DFAA0(barjointnew)`：

```text
从当前选择集取对象
  -> sub_1405C6820
  -> sub_1405E0E70 筛旧钢筋对象
  -> ACIS transaction
  -> 旧 VisualTS steeljoint / steelbar 业务函数
```

工程含义：

```text
barjointnew 更适合采“单根 / 单段钢筋对象接头”样例。
```

`sub_1405F0060(groupjointnew)`：

```text
从当前选择集取对象
  -> sub_1405C6820
  -> sub_1405F17C0 筛旧钢筋组对象
  -> ACIS transaction
  -> sub_1405E9640 等旧组接头业务函数
```

工程含义：

```text
groupjointnew 更适合采“钢筋组接头”样例。
```

`sub_1405EF140(goujianjointnew / featjointnew 系列)`：

```text
从当前选择集取对象
  -> sub_1405C6820
  -> sub_14045A7F0 筛对象
  -> ACIS transaction
```

工程含义：

```text
goujianjointnew / featjointnew 可能面向构件 / 特征对象接头。
当前不应和 barjointnew / groupjointnew 混成一个采样动作。
```

### 中文 UI caption 未直接命中

本轮搜索：

```text
接头
搭接
焊接
相间
定尺
下料
钢筋接头
接头反向
移动接头
清除接头
新建接头
```

结果：

```text
no direct string hit
```

保守结论：

```text
接头相关中文 UI caption 可能来自资源表、Codejock/Ribbon 资源、
外部 UI 数据或非普通字符串缓存。
后续旧界面运行确认仍必须记录真实菜单路径、按钮文字和状态栏提示。
```

本轮不能过度推断：

```text
- 不能据此声明旧 UI 触发路径已经闭合。
- 不能据此声明已采到非空 steeljoint-line / Others 运行样例。
- 不能据此实现真实接头线 / Others 几何算法。
```

## TODO-051 Others / symbolcutIOS producer 与接头 UI 触发静态补证

Evidence ID：

- `E-IDA-033`

本轮使用 IDA MCP 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

本轮目标不是实现接头线算法，也不是启动旧图石采样，
而是在 `TODO-050` 受用户现场运行条件阻塞时，
继续静态缩小 `Others / symbolcutIOS` producer 缺口。

### writer 仍是 sub_14061F970

`sub_14061F970` 当前确认：

```text
callers:
  sub_140600AA0
  sub_140621470
  sub_140625760

callee:
  sub_14053A3F0
```

已有证据确认 `sub_14053A3F0` 写：

```text
SymbolCutIOS%ld
center_x
center_y
center_z
code
```

本轮继续确认 `sub_14061F970` 消费的是 `HVIEWPORT` 风格上下文中的：

```text
HVIEWPORT +840 = symbolcutIOS candidate list
HVIEWPORT +848 = count / non-empty marker
```

### HVIEWPORT 构造函数

`sub_14060E740` 当前确认：

```text
*a1 = HVIEWPORT::vftable
ENTITY_LIST init:
  +776
  +792
  +800
  +808
  +816
  +824

+840:
  sub_14060E470 初始化 ring/list sentinel

+848:
  构造期清零
```

### 直接 producer = sub_14060A810

`sub_14060A810` 当前确认：

```text
node = sub_14060E470(a1)
*(node + 16) = *a4
SPAposition(node + 24, a4 + 8)
```

字段解释：

```text
node + 16 = code
node + 24 = center SPAposition
```

这和 writer 侧 `sub_14053A3F0(center, code, counter)` 对齐。

### 上游 caller = sub_14060C940

`sub_14060A810` 的唯一 caller 是：

```text
sub_14060C940
```

关键链路：

```text
api_entity_entity_distance(...)
if distance <= 0.001:
  api_get_edges(...)
  api_get_vertices(...)
  api_mk_ed_line(...)
  ENTITY_LIST::add(HVIEWPORT +824, edge)
  code   = *(source + 72)
  center = SPAposition(...)
  sub_14060A810(HVIEWPORT +840, sentinel, tail, &var)
  ++*(HVIEWPORT +848)
```

反编译地址线索：

```text
api_entity_entity_distance at 0x14060d5e3
api_mk_ed_line at 0x14060d78a
sub_14060A810 call at 0x14060d7ea
+848 increment at 0x14060d80f
```

`sub_14060C940` 还带源文件字符串：

```text
e:\tushi3d\dam\yxj\yxt.cpp
```

保守结论：

```text
Others / symbolcutIOS 的 producer 链已从 writer 继续追到 HVIEWPORT +840/+848 的直接生产者。
它很可能属于旧图石工程图 / 视图 / 切口符号相关链路。
但当前还不能给完整旧业务功能命名，也不能替代旧图石运行样例。
```

### 接头 UI 触发 stop point

本轮继续复核接头内部命令表：

```text
0x140959328 -> 0x140768480 -> "barjointnew"
0x14095ae70 -> 0x140768480 -> "barjointnew"

0x1409593e8 -> 0x140768548 -> "groupjointrev"
0x14095af90 -> 0x140768548 -> "groupjointrev"

0x14095ae78 -> sub_1405DFAA0
0x14095af98 -> sub_1405F0850
0x14095afa0 -> sub_14054AF20
```

但中文 UI caption 仍未闭合。

继续搜索以下普通字符串仍无直接命中：

```text
接头
搭接
焊接
新建接头
移动接头
清除接头
钢筋接头
```

因此当前只能写：

```text
内部命令和 handler 表有静态证据。
旧 Ribbon / Codejock 中文 caption 或按钮路径仍需静态资源补证或旧图石运行截图确认。
```

本轮不能过度推断：

```text
- 不能声明旧 UI 触发路径已经闭合。
- 不能声明非空 steeljoint-line / Others 运行样例已采到。
- 不能声明真实接头线 / Others 几何算法已实现。
- 不能声明 AutoCAD L2 已通过。
```

## 待继续分析

- `sub_1404DE110` 和 `sub_1404DE720` 已完成第二轮 IDA MCP 补证，公共生成链已追到 `sub_1404D10C0 -> sub_140451730 -> sub_1405D5670 -> sub_1405BD0C0 / sub_1405C7260 / sub_1405E49D0`。
- `sub_1405D5670` 已确认 split / spline / trim / min-distance / 写回主规则，但第 4 个 double 参数来源、字段业务名和对象名仍需继续闭合。
- `TODO-046 / E-IDA-029` 已把 `JointRuler / JointDistbet / JointWeldLength` 的旧参数链、`JointWeldLength / 2000.0` 半长公式、`pattern` raw byte `'L'`、`Others / symbolcutIOS` gate 和额外弧线 / `DrawTaoTong` 关系继续补证；`TODO-050 / E-IDA-032` 又把接头内部命令分成 `barjoint* / groupjoint* / goujianjoint* / featjoint*` 三类采样入口；`TODO-051 / E-IDA-033` 已把 `Others / symbolcutIOS` producer 追到 `sub_14060C940 -> sub_14060A810 -> HVIEWPORT +840/+848 -> sub_14061F970`；但 owning enum / 结构名、旧 UI caption、旧运行非空样例和旧插件接受度仍需继续闭合。
- `TODO-048 / E-IDA-030` 已把旧图石启动期阻塞链闭合到 `sub_1406BBFC0 -> sub_1406BC3B0 -> sub_14070C760`，并确认 `41 -> 许可已过期`、其他非 0 -> `请检查网线是否接好`、`ChaspBase` 许可对象和 `HASP / SuperDog / NetHASP` 许可栈侧证据；`TODO-049 / E-IDA-031` 又补充了 fallback 是宽兜底许可初始化失败文案，不是纯网络专用报错；但当前本机究竟卡在“许可过期 / 网络不可达 / 许可服务未就绪 / 许可文件链异常”的哪一种真实环境原因，仍需用户手工确认。
- `rebarz` / `rebarpost` 业务含义未闭合。
- 顶部 Ribbon 按钮和英文命令并非一一对应，需要结合运行界面确认。
- `生成工程图` 顶部按钮的命令表入口仍需继续追。


## TODO-052 接头 UI caption / Ribbon 绑定静态资源补证

Evidence ID：

- `E-IDA-034`

本轮使用 IDA MCP 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

本轮目标不是实现接头线算法，也不是启动旧图石采样，
而是在 `TODO-051` 已闭合 `Others / symbolcutIOS` producer 静态链后，
继续静态缩小接头 UI caption / Ribbon 绑定缺口。

### 接头 command id 表

当前确认的接头内部命令：

```text
0x8CCE / 36046 -> barjointnew
0x8CCF / 36047 -> barjointclear
0x8CD0 / 36048 -> barjointmove
0x8CD1 / 36049 -> barjointrev

0x8CDD / 36061 -> groupjointnew
0x8CDE / 36062 -> groupjointclear
0x8CDF / 36063 -> groupjointrev
0x8CE0 / 36064 -> groupjointmove

0x8CD9 / 36057 -> goujianjointnew
0x8CDA / 36058 -> goujianjointclear

0x8D91 / 36241 -> segjointnew
0x8D92 / 36242 -> segjointclear

0x8CD7 / 36055 -> featjointnew
0x8CD8 / 36056 -> featjointclear
```

### 接头 handler 表

当前确认：

```text
barjointnew       -> sub_1405DFAA0, aux 0
barjointclear     -> sub_1405DF710, aux 0
barjointmove      -> sub_1405DFEF0, aux 0
barjointrev       -> sub_1405E02D0, aux 0

groupjointnew     -> sub_1405F0060, aux sub_14054AF20
groupjointclear   -> sub_1405EFCC0, aux sub_14054AF20
groupjointrev     -> sub_1405F0850, aux sub_14054AF20
groupjointmove    -> sub_1405F0430, aux sub_14054AF20

featjointnew      -> sub_1405EF140, aux sub_14054AE20
featjointclear    -> sub_1405EEDA0, aux sub_14054AE20

segjointnew       -> sub_1405D94C0, aux sub_14054B410
segjointclear     -> sub_1405D9450, aux sub_14054B410

goujianjointnew   -> sub_1405EF8D0, aux sub_14045A020
goujianjointclear -> sub_1405EF510, aux sub_14045A020
```

关键 xref：

```text
0x140768480 "barjointnew" -> 0x140959328, 0x14095ae70
0x1407684c0 "groupjointnew" -> 0x140959368, 0x14095aed0
0x140768508 "segjointnew" -> 0x1409593a8, 0x14095af30
0x140768528 "featjointnew" -> 0x1409593c8, 0x14095af00
0x140768548 "groupjointrev" -> 0x1409593e8, 0x14095af90
```

### Ribbon 构造函数 stop point

`sub_1406F37B0` 当前确认构造 Codejock Ribbon：

```text
CXTPRibbonBar
CXTPRibbonTab
CXTPRibbonGroup
CXTPControls::Add
```

对照命令 ID 能命中：

```text
36124 / 0x8D1C -> found in sub_1406F37B0
35100 / bdiv   -> found in sub_1406F37B0
35103 / gcom   -> found in sub_1406F37B0
```

接头命令 ID 做 immediate 搜索：

```text
36046,36047,36048,36049,
36061,36062,36063,36064,
36057,36058,
36241,36242,
36055,36056

=> no immediate matches
```

保守结论：

```text
接头命令 ID 未像 0x8D1C / 35100 / 35103 一样，
以普通 immediate 形式直接出现在 sub_1406F37B0 的 Ribbon 构造路径里。

这不证明旧 UI 没有接头入口。
它只说明已检查的顶部 Ribbon 构造函数不能闭合接头中文 caption / Ribbon 绑定。
```

### 字符串和资源证据

普通字符串仍未命中：

```text
接头
搭接
焊接
新建接头
移动接头
清除接头
钢筋接头
```

静态资源能确认接头参数 dialog：

```text
Dialog #427 title = 下料
  定尺长度(米)
  焊接长度(厘米)
  接头相间距离(米)

Dialog #428 title = 创建段组接头
  居中
  首端始
  尾端始
  起始位置
  起点距离
  定尺长度
```

资源证据边界：

```text
Dialog 字段证明旧程序存在接头参数界面。
但它不能证明顶部 Ribbon caption，也不能证明哪个按钮触发 barjointnew / groupjointnew。
```

### TODO-052 stop point

当前最稳妥结论：

```text
接头内部命令和 handler 已确认。
静态 Ribbon 构造函数未直接闭合接头按钮路径。
中文 caption 未从普通字符串直接命中。
接头参数 dialog 存在，但不能替代 UI trigger 证据。
```

后续需要：

```text
继续查右键 / 对象上下文菜单、command dispatch、message map、Codejock command bar 加载路径；
或者由用户在旧图石可运行时截图确认按钮 / 右键菜单 / 弹窗标题。
```

本轮不能过度推断：

```text
- 不能声明旧 UI caption 已闭合。
- 不能声明旧 UI 没有接头入口。
- 不能声明非空 steeljoint-line / Others 运行样例已采到。
- 不能声明真实接头线 / Others 几何算法已实现。
- 不能声明 AutoCAD L2 已通过。
```


## TODO-053 接头 context menu / command dispatch 静态深追

Evidence ID：

- `E-IDA-035`

本轮使用 IDA MCP 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

本轮目标是在 `TODO-052` 已确认接头命令 ID 未直接挂入
`sub_1406F37B0` Ribbon 构造路径后，继续静态查右键 / 对象上下文菜单 /
command dispatch / menu resource 路径。

### 已知接头 command id immediate 搜索

继续检查以下 ID：

```text
0x8CCE / 36046 -> barjointnew
0x8CCF / 36047 -> barjointclear
0x8CD0 / 36048 -> barjointmove
0x8CD1 / 36049 -> barjointrev

0x8CDD / 36061 -> groupjointnew
0x8CDE / 36062 -> groupjointclear
0x8CDF / 36063 -> groupjointrev
0x8CE0 / 36064 -> groupjointmove

0x8CD9 / 36057 -> goujianjointnew
0x8CDA / 36058 -> goujianjointclear

0x8D91 / 36241 -> segjointnew
0x8D92 / 36242 -> segjointclear

0x8CD7 / 36055 -> featjointnew
0x8CD8 / 36056 -> featjointclear
```

结果：

```text
no immediate matches
```

邻近 ID `0x8CDC / 36060` 有命中：

```text
0x1406f4aa8 -> sub_1406F37B0
0x1406f5f6b -> sub_1406F4EC0
```

但 `0x8CDC` 当前不在已确认接头 command id 表中，只能作为邻近 / 可疑命令值记录，
不能归类为接头 UI 入口证据。

### popup / context menu 调用面

菜单 API xref：

```text
CreatePopupMenu -> sub_1405C2EF0, sub_1406BA690
AppendMenuA     -> sub_1405C2EF0, sub_1406BA690
TrackPopupMenu  -> sub_1405C2EF0, sub_1406B1A00, sub_1406BA690, sub_1407072E0
LoadMenuW       -> sub_1406B1A00, sub_1407072E0
```

这证明旧图石有动态 popup / context menu 机制。

### sub_1406BA690

`sub_1406BA690` 当前确认：

```text
ClientToScreen
CreatePopupMenu
AppendMenuA
CMenu::TrackPopupMenu
按 FACE / EDGE / VERTEX 等选择状态构造动态菜单
会递归调用 sub_1405C2EF0
```

当前 decompile 可见大量菜单 ID，包括：

```text
0x8927, 0xE120,
0x8CAC, 0x8AAD, 0x8AAF,
0x8D38, 0x8928, 0x8CE1, 0x8CA4, 0x8DD9,
0x8D40, 0x8D41, 0x8D42, 0x8D43, 0x8E2F, 0x8E30,
0x8CED, 0x8D48, 0x8D49, 0x8DDC, 0x8DDE, 0x8DDB,
0x8E27, 0x8E26, 0x8D22, 0x8D23, 36132, 0x8E32,
36164, 0x8DD4, 0x8D36, 0x8DDD, 0x8DFE, 0x8DDA,
0x8DD3, 0x8DDF, 36343, 36150, 0x8D46, 36140
```

但当前未看到已知接头 command id。

保守结论：

```text
sub_1406BA690 证明旧 VisualTS 有几何选择相关动态 context menu，
但它不能闭合接头命令 UI 入口。
```

### sub_1405C2EF0

`sub_1405C2EF0` 当前确认：

```text
函数大小约 8651 bytes
switch 约 155 cases
cyclomatic complexity = 171
使用 CreatePopupMenu / AppendMenuA / CMenu::TrackPopupMenu
由 sub_1406BA690 调用
```

它是另一个关键动态菜单构造函数。

但全局 immediate 搜索仍显示：

```text
已知接头 command id 没有普通 immediate 命中。
```

因此当前不能声明 `sub_1405C2EF0` 已闭合接头菜单入口。

### LoadMenuW 菜单资源

两个 LoadMenuW 路径：

```text
sub_1406B1A00 -> LoadMenuW(resource 0xAD)
sub_1407072E0 -> LoadMenuW(resource 0xF8)
```

只读解析 `VisualTS.exe` 的 MENU 资源：

```text
menu_resource_count = 15
menu_resource_names =
  0x7e, 0x7f, 0x95, 0x96, 0x97,
  0xa6, 0xa8, 0xa9, 0xad, 0xae,
  0xbc, 0xbd, 0xf8, 0x1ae, 0x3f7
```

资源 `0xAD`：

```text
known joint ids = none
```

资源 `0xF8`：

```text
known joint ids = none
```

全量 MENU 资源：

```text
all_menu_joint_hits_count = 0
```

### handler 表 / .pdata 纠偏

本轮继续确认：

```text
0x14095ae.. = 真实 .data handler 表
0x140a0.... = .pdata unwind metadata
```

`.pdata` 不是业务调度表，不能作为 UI dispatch 证据。

### TODO-053 stop point

当前最稳妥结论：

```text
接头内部 command id / handler 表已确认。
旧图石动态 context menu 机制已确认。
sub_1406BA690 / sub_1405C2EF0 是关键 popup/menu 构造路径。
LoadMenuW 的 0xAD / 0xF8 菜单资源路径已确认。
全量 MENU 资源没有已知接头 command id。
已知接头 command id 没有普通 immediate 命中。
旧 UI caption / 右键菜单项 / command dispatch 绑定仍未闭合。
```

本轮不能过度推断：

```text
- 不能声明旧 UI caption 已闭合。
- 不能声明旧 UI 没有接头入口。
- 不能把 .pdata xref 当 command dispatch 证据。
- 不能把邻近 ID 0x8CDC 当接头证据。
- 不能声明非空 steeljoint-line / Others 运行样例已采到。
- 不能声明真实接头线 / Others 几何算法已实现。
- 不能声明 AutoCAD L2 已通过。
```

后续建议：

```text
TODO-054 / 接头 handler 业务对象筛选链静态分类 P0
```


## TODO-054 接头 handler 业务对象筛选链静态分类

Evidence ID：

- `E-IDA-036`

本轮使用 IDA MCP 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
strings_cache_size = 16320
```

本轮目标是在 `TODO-053` 已把 UI / context menu / dispatch 静态路径追到 stop point 后，
继续确认接头 handler 内部到底筛选什么对象、读什么参数、失败口径是什么。

### handler 表

```text
barjointnew       -> sub_1405DFAA0
barjointclear     -> sub_1405DF710
barjointmove      -> sub_1405DFEF0
barjointrev       -> sub_1405E02D0

groupjointnew     -> sub_1405F0060
groupjointclear   -> sub_1405EFCC0
groupjointrev     -> sub_1405F0850
groupjointmove    -> sub_1405F0430

featjointnew      -> sub_1405EF140
featjointclear    -> sub_1405EEDA0

segjointnew       -> sub_1405D94C0
segjointclear     -> sub_1405D9450

goujianjointnew   -> sub_1405EF8D0
goujianjointclear -> sub_1405EF510
```

### 共同入口和失败口径

多数 handler 共同路径：

```text
sub_1406ED3C0(...) -> 当前 selection/context
*(ctx + 8)          -> ENTITY_LIST
ENTITY_LIST::init / next / count / operator[]
sub_1405C6820(item) -> selection item 有效性过滤
sub_14054C720()     -> 上下文可用性检查，部分批量 handler 使用
sub_14054C760(...)  -> view / ACIS 操作上下文
sub_1406B8140(...)  -> view/update/mark 准备
ACISExceptionCheck("API")
update_from_bb()
view vtable +456(view, obj, 1)
```

失败 / 空选择口径：

```text
ctx null -> return 0
ctx + 8 ENTITY_LIST null -> return 0
过滤后对象数 < 1 -> return 0
view/context null -> return 0
单选命令 count != 1 -> return 0
对象类型 predicate 失败 -> return 0 或跳过
```

### 类型过滤函数

四类业务对象 predicate 都是：

```text
obj && obj->vfunc(+24)(obj, 2) == type_id
```

已确认：

```text
sub_1405E0E70(obj) -> obj type == dword_140993E74
sub_1405F17C0(obj) -> obj type == dword_140993EC8
sub_1405DA020(obj) -> obj type == dword_140993E1C
sub_14045A7F0(obj) -> obj type == dword_1409931D0
sub_1405C6F90(item) -> *(uint32_t *)(item + 80)
```

### barjoint*

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405E0E70(obj)
```

关键字段：

```text
obj + 88  -> 几何 / 显示链头，sub_1405DB6C0 / sub_1405DBE20 遍历。
obj + 96  -> 奇偶控制初始接头位置偏移。
obj + 108 -> sub_1405E1D50 写入，接头周期 / JointRuler 相关字段。
obj + 112 -> sub_1405E1CC0 写入，归一化后的接头位置。
obj + 116 -> sub_1405E1D20 写入，reverse flag。
```

动作分类：

```text
barjointnew:
  dword_140994AB8 >= 1
  JointRuler/1000 < sub_1405DC870(obj,0,0) 时写 obj+108 / +112 / +116，并 sub_1405DB6C0。

barjointclear:
  对每个筛选对象 sub_1405E1D20(obj,0) + sub_1405DBE20(obj)。

barjointmove:
  要求 selection count == 1 且 sub_1405DC6C0(obj) >= 1。
  初值 sub_1405DC6E0(obj)/1000.0，经 sub_14058B8D0 输入后按 obj+108 周期归一化。
  写 obj+112，再 sub_1405DB6C0。

barjointrev:
  要求 selection count == 1 且 sub_1405DC6C0(obj) >= 1。
  读取 obj+116 取反，写回后 sub_1405DB6C0。
```

### groupjoint*

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405F17C0(obj)
```

动作分类：

```text
groupjointnew   -> sub_1405E9640(obj) + view+456
groupjointclear -> sub_1405E7960(obj) + view+456
groupjointrev   -> sub_1405ED6C0(obj) + view+456
groupjointmove  -> 首个对象从 *(obj+80) 取 sub_1405DC6E0 初值，经 sub_14058B8D0 输入后
                   对所有对象调用 sub_1405EBA30(obj, distance) + view+456
```

继续追踪重点：

```text
obj + 80
sub_1405E9640
sub_1405E7960
sub_1405ED6C0
sub_1405EBA30
```

### segjoint*

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405DA020(obj)
```

动作分类：

```text
segjointnew:
  只取 selection 第一个对象。
  operator new(0x148) -> sub_14045D580(dialog,obj,0)
  vtable +728(dialog, 428, 0)
  CWnd::ShowWindow(dialog, 5)

segjointclear:
  只取 selection 第一个对象。
  sub_1405CEB60(obj)
```

`segjointnew` 当前可确认是 Dialog #428 `创建段组接头` 参数入口，不是立即生成接头线。

### featjoint*

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_14045A7F0(obj)
```

动作分类：

```text
featjointnew   -> direct_render_mesh_manager::end_indexed_polygon(obj) + view+456
featjointclear -> sub_140446AE0(obj) + view+456
```

边界：

```text
Hex-Rays 当前把对象显示为 direct_render_mesh_manager*。
这只能说明动作调用链，不能把 owning 业务结构名写死。
```

### goujianjoint*

对象来源特殊：

```text
直接使用 selection item。
不取 *((QWORD*)item + 13)。
```

筛选：

```text
sub_1405C6820(item)
sub_1405C6F90(item) == 4
sub_1405C6F90(item) = *(uint32_t *)(item + 80)
```

内部链：

```text
for (node = *(QWORD *)(item + 120); node; node = *(QWORD *)(node + 128)) {
    obj = *(QWORD *)(node + 104);
}
```

动作分类：

```text
goujianjointnew:
  对 node+104 对象调用 direct_render_mesh_manager::end_indexed_polygon(obj) + view+456。

goujianjointclear:
  对 node+104 对象调用 sub_140446AE0(obj) + view+456。
```

架构影响：

```text
后续新系统选择适配层需要支持“选中构件 item -> 展开内部对象链 -> 批量作用子对象”的语义。
不能把 goujianjoint 简化为普通钢筋对象直接选择。
```

### 本轮 stop point

已闭合：

```text
五组 handler 的对象来源和类型过滤。
barjoint 的主要写回字段 offset 108 / 112 / 116。
groupjointmove 的 obj+80 子对象线索。
segjointnew 到 Dialog #428 的参数入口。
goujianjoint 的 selection item + 内部链表特殊路径。
```

仍未闭合：

```text
groupjoint 动作函数的字段语义。
feat/goujian 路径中 end_indexed_polygon / sub_140446AE0 的准确业务命名。
goujian item+120 / node+104 / node+128 的 owning 结构名。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
真实接头线 / Others 几何算法。
```

后续建议：

```text
TODO-055 / 接头 handler 动作函数字段语义深追 P0
```

## TODO-055 接头 handler 动作函数字段语义深追

Evidence ID：

- `E-IDA-037`

本轮使用 IDA MCP 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
strings_cache_size = 16320
```

本轮目标是在 `TODO-054` 已确认接头 handler 对象筛选链之后，继续追动作函数的字段访问、写回规则和剩余 stop point。

### 覆盖函数

```text
sub_1405E9640  -> groupjointnew 动作函数
sub_1405E7960  -> groupjointclear / feat-goujian 间接 clear 共用函数
sub_1405ED6C0  -> groupjointrev 动作函数
sub_1405EBA30  -> groupjointmove 动作函数
sub_140446AE0  -> featjointclear / goujianjointclear 的子对象 clear adapter
sub_1405CEB60  -> segjointclear 动作函数
sub_14045D580  -> segjointnew Dialog #428 初始化函数
```

### groupjoint 字段语义

```text
groupObj + 80 -> 子接头对象链头。

child + 72  -> 同组下一个子接头对象。
child + 88  -> 已生成接头几何 / 显示链头，clear / rebuild 会删除。
child + 96  -> 奇偶错位选择字段，影响 JointDistbet 修正。
child + 108 -> JointRuler / 接头周期，旧代码按 int mm 存储。
child + 112 -> 当前接头相位 / 位置，写入前按周期归一化。
child + 116 -> reverse flag。
child + 128 -> 低层 ACIS/HOOPS 生成实体链，删除时会 HA_Delete_Entity_Geometry / api_del_entity。
```

### 写回 helper

`sub_1405E1D50(child, value)`：

```text
ENTITY::backup(child)
*(int *)(child + 108) = value
```

`sub_1405E1CC0(child, value)`：

```text
period = *(int *)(child + 108)
phase = value

if abs(value) >= 1:
  if period > 1:
    while phase < 1:
      phase += period
    while phase > period:
      phase -= period
else:
  phase = period

ENTITY::backup(child)
*(int *)(child + 112) = phase
```

`sub_1405E1D20(child, flag)`：

```text
ENTITY::backup(child)
*(byte *)(child + 116) = flag
```

`sub_1405DC6E0(child)`：

```text
period = *(int *)(child + 108)
phase = *(uint32 *)(child + 112)
if period:
  return phase % period
return phase
```

工程含义：

```text
旧图石每次写 +108 / +112 / +116 前都会调用 ENTITY::backup。
后续复刻不能把这些动作简化为普通字段赋值；至少要保留 dirty / undo / transaction 语义位置。
```

### sub_1405E9640 / groupjointnew

```text
if dword_140994AB8 < 1:
  return 0

if dword_140994AB8 < dword_14095D628:
  dword_14095D628 = dword_140994AB8 / 2

periodM = dword_140994AB8 / 1000.0
sub_1405E7960(groupObj)

for child = *(groupObj+80); child; child = *(child+72):
  if periodM < sub_1405DC870(child, 0, 0):
    sub_1405E1D50(child, dword_140994AB8)
    phase = dword_140994AB8
    if *(int *)(child+96) % 2 < 1:
      phase = dword_140994AB8 - dword_14095D628
    sub_1405E1CC0(child, phase)
    sub_1405E1D20(child, 0)
    sub_1405DB6C0(child)
return 1
```

### sub_1405E7960 / groupjointclear

```text
for child = *(groupObj+80); child; child = *(child+72):
  sub_1405E1D20(child, 0)
  sub_1405DBE20(child)
return 1
```

`sub_1405DBE20(child)`：

```text
for geom = *(child+88); geom; geom = *(geom+88):
  sub_1405B9640(geom)
```

### sub_1405ED6C0 / groupjointrev

```text
for child = *(groupObj+80); child; child = *(child+72):
  sub_1405E1D20(child, *(byte *)(child+116) == 0)
  sub_1405DB6C0(child)
return 1
```

### sub_1405EBA30 / groupjointmove

```text
child = *(groupObj+80)
if !child:
  return 0

while child:
  period = *(int *)(child+108)
  if period >= 1 and period/1000.0 < sub_1405DC870(child, 0, 0):
    phase = inputDistanceM * 1000.0 + 0.5
    if *(int *)(child+96) % 2 < 1:
      phase -= dword_14095D628
    sub_1405E1CC0(child, (int)phase)
    sub_1405E1D20(child, 0)
    sub_1405DB6C0(child)
  child = *(child+72)
return 1
```

`groupjointmove / sub_1405F0430` 入口补证：

```text
首个 groupjoint 对象：
  initialDistance = sub_1405DC6E0(*(groupObj+80)) / 1000.0
  sub_14058B8D0(..., &initialDistance)

随后对所有 groupjoint 对象：
  sub_1405EBA30(groupObj, initialDistance)
```

### sub_1405DB6C0 / 几何重建入口

关键字段驱动：

```text
sub_1405DBE20(child)
period = *(int *)(child+108)
periodM = period / 1000.0
phaseM = sub_1405DC6E0(child) / 1000.0
if phaseM < 0.02:
  phaseM = periodM
if *(byte *)(child+116):
  从反向端开始遍历
遍历 child+88 几何链 / EDGE
按 EDGE::length / get_bounded_curve / bounded_curve vfunc(+64) 取点
调用 direct_render_mesh_manager::end_indexed_polygon(...)
```

工程含义：

```text
sub_1405DB6C0 是按字段重建接头几何的核心入口。
本轮只确认字段驱动、清旧链、遍历和输出调用点；完整几何算法留给 TODO-056。
```

### sub_1405B9640 / 底层几何链删除

```text
for node = *(obj+128); node; node = *(node+88):
  HA_Delete_Entity_Geometry(node)
  api_del_entity(*(node+64))
  backup(node); *(node+64) = 0
  backup(node); *(node+80) = 0
  backup(node); *(node+88) = 0
  api_del_entity(node)

backup(obj); *(obj+128) = 0
backup(obj); *(obj+112) = 0
```

工程含义：

```text
旧 clear / rebuild 包含 ACIS/HOOPS entity 删除和字段清零，不是简单 UI 隐藏。
```

### sub_140446AE0 / feat-goujian clear adapter

```text
for subObj = *(obj+192); subObj; subObj = *(subObj+88):
  sub_1405E7960(subObj)
return lastResult
```

handler 差异：

```text
featjointclear:
  selection item +13 -> feat object -> sub_140446AE0(featObj) -> view+456

goujianjointclear:
  selection item +80 == 4
  item+120 child node chain
  node+104 -> action object -> sub_140446AE0(actionObj) -> view+456
```

补充：

```text
featjointnew / sub_1405EF140 过滤 feat object 后直接调用 direct_render_mesh_manager::end_indexed_polygon(featObj) + view+456。
它不经过 sub_140446AE0。
```

### sub_1405CEB60 / segjointclear

```text
for item = *(segObj+80); item; item = *(item+96):
  sub_1405B9640(item)
return lastResult
```

工程含义：

```text
segjointclear 清理 segObj+80 下的段组接头子链。
该子链 next 字段是 +96，不是 groupjoint 子对象的 +72。
```

### sub_14045D580 / segjointnew Dialog #428 初始化

```text
CDialog::CDialog(dialog, 0x1AC, parent)  // 0x1AC = 428
*(dialog+320) = segObj
vtable = JoingSegDlg::vftable
*(int *)(dialog+308) = dword_140994AB8
*(int *)(dialog+304) = 0
*(int *)(dialog+312) = dword_140994AB8
```

工程含义：

```text
segjointnew 当前可确认是创建 JoingSegDlg / Dialog #428 参数窗口。
Dialog 保存 segObj 到 +320，并把 JointRuler 默认值写入 +308 / +312。
本轮不追 Dialog 确定按钮后的真实生成逻辑。
```

### 本轮 stop point

已闭合：

```text
TODO-055 指定 7 个函数的字段访问和写回规则。
groupjoint 子链头 / next / phase / period / reverse 字段。
feat/goujian clear 的 obj+192 子链 adapter。
segjoint clear 的 obj+80 / +96 子链。
segjointnew Dialog #428 初始化字段。
写回 helper 会调用 ENTITY::backup。
```

仍未闭合：

```text
sub_1405DB6C0 内部完整几何算法。
JoingSegDlg Dialog #428 确定按钮后的 segjoint 创建链。
feat/goujian obj+192 子链 owning 结构名。
goujian item+120 / node+104 / node+128 owning 结构名。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
golden。
```

后续建议：

```text
TODO-056 / 接头重建几何核心 sub_1405DB6C0 静态深追 P0
```

## TODO-056 接头重建几何核心 sub_1405DB6C0 静态深追

证据编号：

```text
E-IDA-038
```

IDA 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
```

本轮覆盖函数：

```text
sub_1405DB6C0  -> 接头生成链几何重建核心
sub_1405DBE20  -> 清 child+88 旧生成链
sub_1405DC6E0  -> 读取 child+112 % child+108
sub_1405DC840  -> 沿 child+88 / node+88 链找尾节点
sub_1405E06F0  -> 统计 child+88 / node+88 链节点数
sub_1405E05A0  -> 取末端参考点
sub_1405E1E70  -> 取起端参考点
sub_1405D36D0  -> 多段链连接点 / 位置分类 helper
sub_14054C2C0  -> 两条 EDGE 的连接点 / 最近点 helper
sub_14054C820  -> 两条 EDGE 的保守端点连接 helper
sub_1405BBEF0  -> 沿 owner/head 的 node+88 链找当前节点前驱
sub_1405DC870  -> 旧链总长度估算 helper
sub_1405DC6C0  -> 累加生成链节点 +112
```

### sub_1405DB6C0 主流程

```text
sub_1405DBE20(child)             // 先清旧生成链
node = *(child+88)
period = *(int *)(child+108)     // mm
phase = sub_1405DC6E0(child)     // child+112 % child+108, mm
periodM = period / 1000.0
phaseM = phase / 1000.0
if phaseM < 0.02:
  phaseM = periodM

if child+116 reverse:
  node = sub_1405DC840(child)    // 找尾节点

while node:
  edge = *(node+72)
  directionFlag = calc via single/multi chain reference points
  edgeLen = EDGE::length(edge, 1)

  if phaseM <= accumulated + edgeLen:
    bounded_curve = get_bounded_curve(edge, 1)
    scale = linear ? 1.0 : (paramEnd - paramStart) / edgeLen
    curveParam = start + distance*scale OR end - distance*scale
    point = bounded_curve.vfunc(+64)(curveParam)
    index = int(phaseM * 1000 + 0.5)
    end_indexed_polygon(node, point, index)
    phaseM = periodM for subsequent points

  node = reverse ? sub_1405BBEF0(node) : *(node+88)
```

关键常量：

```text
0x1408bbab8 = 1000.0
0x14075a798 = 0.02
0x1408bba30 = 0.5
0x1408bba40 = 1.0
```

### end_indexed_polygon 调用点

Hex-Rays 对 `direct_render_mesh_manager::end_indexed_polygon` 的原型不完整。

以 `sub_1405DB6C0` 调用点寄存器为准：

```text
0x1405dbb1b  mov r8d, ebx       -> int mm index
0x1405dbb1e  mov rcx, rsi       -> generated-chain node
0x1405dbb21  mov rdx, rax       -> SPAposition*
0x1405dbb24  call direct_render_mesh_manager::end_indexed_polygon
```

结论：

```text
rcx = generated-chain node
rdx = SPAposition*
r8d = int mm index
```

不能声明完整类原型已恢复，只能声明该调用点语义已确认。

### helper 语义

```text
sub_1405DBE20(child):
  遍历 child+88 / node+88，逐个 sub_1405B9640(node)，用于重建前清旧链。

sub_1405DC6E0(child):
  period = *(int *)(child+108)
  phase = *(uint *)(child+112)
  return period ? phase % period : phase

sub_1405DC840(child):
  从 child+88 沿 node+88 找尾节点。

sub_1405BBEF0(node):
  从 *(node+80)+88 的链头开始找当前 node 的前驱。

sub_1405E06F0(child):
  统计 child+88 / node+88 节点数，DB6C0 用它区分单段链和多段链。

sub_1405E05A0(child):
  无链返回 0,0,0；单节点返回 *(child+80)+192；多节点用末端 node+104 调 sub_1405D36D0。

sub_1405E1E70(child):
  无链返回 0,0,0；单节点返回 *(child+80)+168；多节点用首节点 node+104 调 sub_1405D36D0。

sub_1405D36D0:
  返回 0/1/2/3 或 -1 的位置分类。
  分类 1/2 调 sub_14054C820。
  分类 3 且有当前 node 时，按 a4 选择前驱或后继 EDGE，再调 sub_14054C2C0。

sub_14054C2C0:
  先判断四种端点 same_point。
  a4 为真时可调用 api_entity_entity_distance 得到最近点。
  a4 为假时在四个端点距离里选最近端点。

sub_14054C820:
  保守地在两条 EDGE 的端点之间选连接点。
```

### DB6C0 code callers

```text
sub_1405DFAA0  -> barjointnew 类批量新建入口，写 +108/+112/+116 后 rebuild
sub_1405DFEF0  -> barjointmove 类单对象移动入口，写 +112 后 rebuild
sub_1405E02D0  -> barjointrev 类单对象反向入口，切换 +116 后 rebuild
sub_1405E9640  -> groupjointnew，逐 child 写 period/phase/reverse 后 rebuild
sub_1405EBA30  -> groupjointmove，逐 child 写 phase/reverse 后 rebuild
sub_1405ED6C0  -> groupjointrev，逐 child 切换 reverse 后 rebuild
```

### 字段语义更新

```text
child+88  = generated chain head
child+108 = period / JointRuler, int mm
child+112 = phase / position, int mm modulo period
child+116 = reverse flag

node+72   = EDGE*
node+80   = owner/head for predecessor lookup
node+88   = next generated-chain node
node+104  = multi-edge connection point participant
node+112  = summed by sub_1405DC6C0; business name still open
```

### 本轮 stop point

已闭合：

```text
DB6C0 的清旧链、period/phase/reverse 字段驱动。
正向 / 反向遍历方式。
单段 / 多段链连接点 helper。
EDGE::length + get_bounded_curve + bounded_curve vfunc(+64) 取点。
linear edge scale = 1.0，非线性 edge 用参数区间 / 长度换算。
end_indexed_polygon 调用点的 rcx/rdx/r8d 语义。
barjoint/groupjoint new/move/rev caller 集合。
```

仍未闭合：

```text
DB6C0 owning 结构名和 node+112 业务名。
JoingSegDlg Dialog #428 确定按钮后的 segjoint 创建链。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
真实接头线算法。
真实 Others 几何算法。
golden。
```

后续建议：

```text
TODO-057 / 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0
```

## TODO-057 接头 DB6C0 owning 结构与 Dialog #428 确定链补证

证据编号：

```text
E-IDA-039
```

IDA 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
```

### type id -> debug 类名

静态 debug name 已补齐：

```text
sub_1405E0EA0:
  dword_140993E74 -> "steelbar"
  source path     -> e:\tushi3d\dam\class\steelbar.cpp

sub_1405F1820:
  dword_140993EC8 -> "steelbargroup"
  source path      -> e:\tushi3d\dam\class\steelbargroup.cpp

sub_1405DA0E0:
  dword_140993E1C -> "seg_steelbargroup"
  source path      -> e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

工程含义：

```text
barjoint*   操作 steelbar。
groupjoint* 操作 steelbargroup，并沿 groupObj+80 子链逐 child 重建。
segjointnew 操作 seg_steelbargroup，并先进入 Dialog #428。
```

### Dialog #428 打开链

```text
sub_1405D94C0:
  ctx = sub_1406ED3C0(...)
  first selected item
  obj = *((QWORD*)item + 13)
  if sub_1405DA020(obj):      // seg_steelbargroup
    dialog = operator new(0x148)
    sub_14045D580(dialog, obj, 0)
    vtable+728(dialog, 428, 0)
    ShowWindow(dialog, 5)
```

### Dialog #428 构造 / DDX / radio / init

```text
sub_14045D580:
  CDialog::CDialog(dialog, 0x1AC, parent)  // 428
  dialog+320 = segObj
  dialog+304 = 0
  dialog+308 = dword_140994AB8
  dialog+312 = dword_140994AB8
  vtable = JoingSegDlg::vftable

sub_14045D650:
  DDX_Radio(..., 1380, dialog+304)
  DDX_Text (..., 1426, dialog+308)
  DDX_Text (..., 1283, dialog+312)
  DDV_MinMaxFloat(..., 0.01, 15000.0)

sub_14045DA00 / sub_14045DA10 / sub_14045DA20:
  dialog+304 = 0 / 1 / 2

sub_14045DFF0:
  调整窗口和按钮位置
  隐藏 1261/1262/1263/1264、1330/1331/1332/1333、
       1402/1403/1404/1405、1481/1479/1480
  最后 CDialog::OnInitDialog(this)
```

### Dialog #428 OnOK 链

```text
sub_14045D720:
  ACISExceptionCheck("API")
  sub_1405CB160(
    dialog+320,  // segObj
    dialog+304,  // mode
    dialog+308,  // firstOffset / phase candidate
    dialog+312)  // period
  CDialog::OnOK(this)
  view refresh chain
```

### segjoint 创建链

```text
sub_1405CB160(segObj, a2, a3, a4):
  node = *(segObj+80)
  for i = 0; node; ++i:
    phase = a3
    if i is odd:
      phase = 0
    sub_1405B7350(node, a2, phase, a4)
    node = *(node+96)

sub_1405B7350(node, a2, a3, a4):
  a3 / a4 先 /1000.0 转米
  if a3 < 0.02:
    a3 = a4
  edge = *(node+72)
  edgeLen = EDGE::length(edge, 1)
  bounded = get_bounded_curve(edge, 1)
  closed edge:
    从 a3 开始每隔 a4 取点，end_indexed_polygon(this)
  open edge:
    先做端点方向判断，再按 mode 取一端起排或中分起排
    每次 bounded_curve vfunc(+64) 取点，end_indexed_polygon(this)
```

与 DB6C0 的共同常量：

```text
0.02
1000.0
0.5
1.0
```

工程含义：

```text
Dialog #428 OK 后不是直接进 DB6C0。
它沿 seg_steelbargroup.obj+80 子链逐节点分发，
并走与 DB6C0 接近的起始偏移 / 周期 / bounded curve 取点语义。
```

### child+112 与 generated node+112 的边界

```text
child+112:
  -> sub_1405E1CC0 写回
  -> 按 child+108 period 做归一化
  -> sub_1405DC6E0(child) = child+112 % child+108
  -> 当前最稳妥业务名：
       phase / position / 起始偏移，int mm

generated node+112:
  -> TODO-056 已确认 sub_1405DC6C0 会累加它
  -> 业务名仍未闭合
```

### Apply / message map stop point

对以下函数做 xref 复核：

```text
sub_14045D720
sub_14045D650
sub_14045DFF0
sub_14045DA00
sub_14045DA10
sub_14045DA20
```

结果：

```text
xrefs 只稳定命中 data / vtable / message-map-like 数据引用。
当前没有独立代码 xref 把 standalone Apply handler 单独钉出来。
```

结论：

```text
当前能高置信声明：
  sub_14045D720 = JoingSegDlg::OnOK

当前不能高置信声明：
  JoingSegDlg 已存在一个独立定位的 Apply handler
  或 Apply 与 OK 的复用关系已静态闭合
```

### 本轮 stop point

已闭合：

```text
dword_140993E74 / dword_140993EC8 / dword_140993E1C 的类名。
barjoint / groupjoint / segjoint 的 owner 分层。
Dialog #428 的打开链、构造字段、DDX、radio、OnInit、OnOK。
sub_1405CB160 -> sub_1405B7350 的 per-node 确定链。
child+112 的业务语义可收紧为 phase / position / 起始偏移。
```

仍未闭合：

```text
groupjoint child 的精确业务类名。
generated node+112 的业务名。
Dialog #428 standalone Apply handler。
旧 UI caption / 右键菜单项绑定。
旧图石非空运行样例。
AutoCAD L2 接受度。
真实接头线算法。
真实 Others 几何算法。
golden。
```

后续建议：

```text
TODO-058 / JoingSegDlg message map 与 child/node+112 字段收口 P1
```

## TODO-058 JoingSegDlg message map 与 child/node+112 字段收口

证据编号：

```text
E-IDA-040
```

IDA 会话：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
```

### JoingSegDlg message map 静态表

静态链：

```text
sub_14045D6F0 -> sub_14045D700 -> &off_14075B110

off_14075B110:
  +0x00 -> 0x140709724 = CDialog::GetThisMessageMap
  +0x08 -> 0x14075B090 = JoingSegDlg message entries
```

`0x14075B090` 的 entries 可按 32 字节一项稳定解码为：

```text
entry0:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0564 = 1380
  lastId  = 0x0564 = 1380
  sig     = 0x3A
  handler = sub_14045DA00 -> dialog+304 = 0

entry1:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0568 = 1384
  lastId  = 0x0568 = 1384
  sig     = 0x3A
  handler = sub_14045DA10 -> dialog+304 = 1

entry2:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0565 = 1381
  lastId  = 0x0565 = 1381
  sig     = 0x3A
  handler = sub_14045DA20 -> dialog+304 = 2

terminator:
  all zero
```

工程含义：

```text
JoingSegDlg 自己的静态 message map 当前只闭合出 3 个 radio 模式切换命令。
它们与 sub_14045D650 的 DDX_Radio(control 1380) 以及
sub_14045DA00 / DA10 / DA20 的 mode=0/1/2 写回完全对齐。
```

### Apply / OK / Cancel 的静态边界

本轮复核点：

```text
sub_14045D700   -> 返回 message map
sub_14045D720   -> OnOK
sub_14045DFF0   -> OnInitDialog
sub_14045DA00   -> radio 0
sub_14045DA10   -> radio 1
sub_14045DA20   -> radio 2
```

高置信结论：

```text
1. message map 里没有单独的 WM_COMMAND entry 命中：
   IDOK(1)
   IDCANCEL(2)
   Apply-like 自定义按钮 id

2. sub_14045DFF0 只明确操作：
   GetDlgItem(1) / GetDlgItem(2)   // 默认 OK / Cancel 位置调整
   以及一批参数相关控件的 ShowWindow(0)

3. sub_14045D720 已明确是 JoingSegDlg::OnOK。
```

当前最稳妥的 stop point：

```text
可以高置信说：
  Dialog #428 走默认 OK / Cancel 框架，
  JoingSegDlg 自己额外声明了 3 个 radio handler，
  但静态上没有独立 Apply handler 证据。

还不能高置信说：
  旧界面绝对不存在 Apply 按钮
  或某个运行期构造按钮一定复用/分离于 OnOK
```

### Dialog #428 的资源 / 控件 xref 边界

当前已能稳定闭合的 dialog 资源级线索：

```text
sub_14045D580:
  CDialog::CDialog(dialog, 0x1AC, parent)   // 0x1AC = 428

sub_14045D650:
  DDX_Radio(..., 1380, dialog+304)
  DDX_Text (..., 1426, dialog+308)
  DDX_Text (..., 1283, dialog+312)
  DDV_MinMaxFloat(..., 0.01, 15000.0)

sub_14045DFF0:
  GetDlgItem(1) / GetDlgItem(2)
  ShowWindow(0):
    1261/1262/1263/1264
    1330/1331/1332/1333
    1402/1403/1404/1405
    1481/1479/1480
```

工程含义：

```text
Dialog #428 的参数区、radio 控件和一批可隐藏扩展控件已经有稳定控件 id 证据，
但这些资源级 xref 仍不能直接推出旧中文 caption、
右键菜单文案或运行期按钮文字。
```

### child+112 与 generated node+112 的最终静态边界

`child+112` 继续保持前一轮结论：

```text
child+112:
  -> 写回路径：sub_1405E1CC0
  -> 读取路径：sub_1405DC6E0(child) = child+112 % child+108
  -> 当前最稳妥业务边界：
       phase / position / 起始偏移，int mm，按 period 归一化
```

本轮把 `generated node+112` 再收紧一层：

```text
sub_1405DC6C0(a1):
  generated = *(a1+88)
  sum += generatedNode+112

caller coverage:
  sub_1405DBC20  -> 用 sum(generated node+112) 决定字符串拼接路径
  sub_1405DFEF0  -> 先要求 sum(generated node+112) >= 1，之后做 phase 写回再 DB6C0
  sub_1405E02D0  -> 先要求 sum(generated node+112) >= 1，之后 clear/rebuild
  sub_1405EAEC0  -> 对 group/seg 子链继续累加 sub_1405DC6C0(child)
  sub_1406F72A0  -> 状态栏/展示路径里读取 sub_1405DC6C0
```

所以当前能高置信声明：

```text
generated node+112:
  -> 位于 child+88 生成链节点上
  -> 是一个 int mm 贡献字段
  -> 会被 sub_1405DC6C0 汇总
  -> 汇总值会进入字符串格式化、状态显示和 rebuild 前置判断
```

当前仍不能高置信声明：

```text
generated node+112 的旧业务调试名
generated node+112 在旧 UI 上的最终中文展示名称
```

因此本轮最终口径是：

```text
child+112
  = dialog/业务写回的 phase / position / 起始偏移

generated node+112
  = 生成链节点上的 int mm 贡献字段，供聚合/显示/判断使用
  = 业务调试名仍未闭合
```

### 最小运行确认清单

如果后续用户现场可以运行旧图石，最小补证清单收敛为：

```text
1. 打开 segjointnew / Dialog #428，
   截全窗口，必须包含标题栏和底部按钮区。

2. 确认底部按钮：
   只有 OK/Cancel，还是还有 Apply / 应用 / 预览类按钮。

3. 切换 3 个 radio 模式，
   记录是否存在“不点确定也立即生效”的行为。

4. 输入两个数值框后确定，
   重新打开同一对象，记录参数是否回显，
   并截图状态栏/尺寸串是否变化。
```

### 本轮 stop point

已闭合：

```text
JoingSegDlg 自身静态 message map。
3 个 radio handler 的 id / handler / mode 写回关系。
Dialog #428 资源控件 id 的静态边界。
child+112 与 generated node+112 的分层边界。
standalone Apply 未在 JoingSegDlg 自身静态表中出现。
```

仍未闭合：

```text
generated node+112 的最终业务调试名。
旧中文 caption / 右键菜单项绑定。
运行期是否存在静态表外的 Apply 形态按钮。
旧图石非空运行样例。
AutoCAD L2 通过。
真实接头线算法。
真实 Others 几何算法。
golden。
```

下一步登记为：

```text
TODO-059 / generated node+112 展示/状态栏字符串链静态深追 P1
```

## TODO-059 追溯补充

`E-IDA-041 / E-DEV-081` 追加关联：

```text
REQ-DRAW-003
REQ-PROJ-001
REQ-TECH-002
GAP-DRAW-002
GAP-DEV-010
```

本轮继续沿 `sub_1405DC6C0` 的 caller / 字符串 / 状态栏链静态深追
`generated node+112` 的展示语义。

## IDA MCP 会话

```text
database = visualts_i64_todo051
module = VisualTS.exe
hexrays_ready = true
strings_cache_size = 16320
```

本轮补看的函数：

```text
sub_1405DBC20
sub_1406F72A0
sub_1405EAEC0
sub_1405DFEF0
sub_1405E02D0
sub_140601D80
sub_1405DBE50
sub_1405DC6A0
sub_1405EAF40
sub_1405E0660
sub_1405DC6C0
sub_1405DC870
sub_1404554B0
sub_1406B4670
```

### `sub_1405DC6C0` 的核心语义

当前已经可以把 `sub_1405DC6C0(a1)` 收紧为：

```text
result = 0
for each generatedNode in a1+88 chain:
  result += generatedNode+112
return result
```

也就是说：

```text
generated node+112
  -> 不是 child+112 的同一字段
  -> 它位于 child+88 生成链节点上
  -> sub_1405DC6C0 汇总的是整条生成链的 node+112 总和
```

### `sub_1405DBC20`：单下料长表达式格式化

当前高置信伪代码：

```text
baseMm = round(sub_1405DC870(a1, 0, 0) * 1000.0)

if sub_1405DC6C0(a1) < 1:
  format "%d"
else:
  for each generatedNode in a1+88 chain:
    for each pieceNode in generatedNode+128 chain:
      pieceMm = *(int *)(pieceNode + 76)
      append "%d+" / "%s%d+" / "%s%d"
```

当前最稳妥工程口径：

```text
sub_1405DBC20
  -> 不是简单打印 generated node+112
  -> 它用 sum(generated node+112) 决定是否进入分段拼接模式
  -> 最终输出更像“单下料长(mm)”的分解表达式
     例如 5000+5000+3200 这种串
```

### `sub_140601D80`：Excel 下料表写出链

本轮把 `sub_140601D80` 收紧到旧 Excel 输出路径：

```text
title = 钢筋下料表

A2 = 编号
B2 = 直径(mm)
C2 = 单净长(mm)
D2 = 焊长(mm)
E2 = 焊头(个)
F2 = 单下料长(mm)
G2 = 根数(根)
H2 = 总根数(根)
```

关键字段落点：

```text
E 列 = sub_1405DC6C0(obj)
F 列 = sub_1405DBC20(obj, buffer)
```

所以本轮可以把展示语义继续收紧为：

```text
sub_1405DC6C0(sum generated node+112)
  -> 在旧 Excel 下料表里直接落到“焊头(个)”

sub_1405DBC20(...)
  -> 在旧 Excel 下料表里直接落到“单下料长(mm)”
```

### `sub_1406F72A0`：状态栏展示链

本轮把状态栏路径也追实了：

```text
pane1:
  编号: %d号钢筋
  编号: %d%c号钢筋
  编号: %d号钢筋 首尾封闭

pane2:
  %s级钢  直径:%s 毫米
  special type 158 -> 型号%d-%g

pane3:
  special type 158 -> 至中心线距离:  %d毫米
  if sub_1405DC6C0(a2) > 0:
    override with one of:
      焊接
      绑扎
      套筒连接

pane4:
  钢筋长度:%g 米
  钢筋长度:%g~%g 米
```

当前最重要的静态收口是：

```text
当 sum(generated node+112) > 0 时，
旧状态栏第 3 格会切换到连接方式文本：
  焊接 / 绑扎 / 套筒连接
```

连接方式选择分支来自：

```text
*(sub_1405E0660(a2) + 64)
```

但这个字段的最终业务名 / enum 命名本轮仍未完全闭合。

### 向上聚合 helper

`sub_1405EAEC0 / sub_1404554B0` 也已补实：

```text
sub_1405EAEC0(a1)
  -> sum sub_1405DC6C0(child) over a1+80 chain

sub_1404554B0(a1)
  -> sum sub_1405EAEC0(v1) over a1+192 chain
```

工程含义：

```text
旧系统不只在单对象上看这个计数，
还会把它沿 group / seg / owner 继续聚合。
```

### 本轮最终收口

到 `TODO-059` 为止，当前最稳妥口径是：

```text
generated node+112
  -> child+88 生成链节点上的整数计数字段
  -> 被 sub_1405DC6C0 汇总
  -> 汇总值在旧 Excel 下料表中写入“焊头(个)”
  -> 汇总值 > 0 时会让旧状态栏第 3 格显示
     焊接 / 绑扎 / 套筒连接
  -> 同时它还是单下料长分段表达式和 rebuild 前置判断的 gate
```

大白话就是：

```text
现在已经不能只把它说成“一个 +112 字段”了。

更接近事实的说法是：
它是一条生成链上的接头 / 焊头计数字段，
旧系统会把它汇总后拿去：
  1. 写 Excel 的“焊头(个)”
  2. 决定“单下料长(mm)”是否走分段拼接串
  3. 在状态栏显示 焊接 / 绑扎 / 套筒连接
```

### 本轮仍不写死的部分

当前仍不能高置信写死为：

```text
generated node+112 的最终旧调试符号名
*(sub_1405E0660(a2) + 64) 的最终业务字段名和完整 enum 名称
旧运行时每种连接方式与该字段具体取值的一一映射
旧图石真实界面截图级闭环
```

所以本轮最终口径仍然是：

```text
可以把 generated node+112 收紧到
“接头 / 焊头计数展示链字段”

但还不把它最终调试名写死。
```

### 本轮 stop point

已闭合：

```text
sub_1405DC6C0 = sum(generated node+112)
Excel 下料表 E 列 = 焊头(个)
Excel 下料表 F 列 = 单下料长(mm) 分解表达式
状态栏 pane3 在 sum > 0 时显示 焊接 / 绑扎 / 套筒连接
group / seg / owner 向上聚合 helper
```

仍未闭合：

```text
generated node+112 的最终业务调试名
连接方式来源字段 *(sub_1405E0660(a2)+64) 的最终命名
旧图石真实运行截图和导出样例
AutoCAD L2
真实接头线算法
真实 Others 几何算法
golden
```

下一步登记为：

```text
TODO-060 / generated node+112 旧图石运行确认与 Excel/状态栏对照 P0
```
