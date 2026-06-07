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

命令 ID 表：

```text
0x14095A1C8 -> 0x140768FF8 "psexcel"
0x14095A1D0 -> 0x8D1C
```

同表相邻项：

```text
0x14095A1D8 -> 0x140769000 "psallc"
0x14095A1E0 -> 0x8CD2
0x14095A1E8 -> 0x140769008 "ysteelout"
0x14095A1F0 -> 0x8CDB
```

handler 表：

```text
0x14095B698 -> 0x140768FF8 "psexcel"
0x14095B6A0 -> sub_140605B20
0x14095B6A8 -> 0
```

同表相邻项：

```text
0x14095B6B0 -> 0x140769000 "psallc"
0x14095B6B8 -> sub_140600AA0
0x14095B6C0 -> 0
```

handler 链：

```text
psexcel
  -> sub_140605B20
  -> sub_140602F90
  -> sub_140601D80
```

已确认含义：

- `psexcel` 在内部命令 ID 表中绑定 `0x8D1C`。
- `psexcel` 在内部 handler 表中绑定 `sub_140605B20`。
- `sub_140605B20` 做权限 / 模型状态检查后，调用 `sub_140602AE0` 查找模板，再调用 `sub_140602F90`。
- `sub_140602F90` 已由 `E-IDA-019` 确认写出 `StbTable / MaterialTable` 字段。
- `sub_140601D80` 引用 `钢筋下料表` 字符串，并属于 `excelcommand.cpp` 导出链。

本轮不能过度推断：

- 尚不能证明顶部 Ribbon 文本 `下料表` 一定直接绑定 `psexcel / 0x8D1C`。
- 尚不能证明 Dialog #427 一定由 `psexcel` 打开。
- 尚不能证明运行时点击 `下料表` 的输出目录、文件名和覆盖行为。

结论：

```text
内部命令绑定已确认：

psexcel / 0x8D1C / sub_140605B20

但 UI 顶部按钮、Dialog #427 和运行输出仍需旧图石运行确认。
```

### 下料表 Ribbon UI 绑定补证

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
- 它向 `输出` 分组加入命令 ID `36124`，也就是 `0x8D1C`。
- 同一函数还把 `36124 / 0x8D1C` 加入 QuickAccess 控件。

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
  psexcel -> 0x8D1C -> sub_140605B20

E-IDA-021:
  工程图 / 输出 Ribbon 分组 -> 0x8D1C
```

已确认含义：

- `0x8D1C` 已不是孤立内部命令 ID。
- 它确实被旧程序 Ribbon 构造函数挂到 `工程图 / 输出` 区域。
- 结合 `E-IDA-020`，`工程图 / 输出` 下的该按钮高度可信地指向 `psexcel / sub_140605B20` 下料链。

本轮不能过度推断：

- 还不能单靠本证据证明按钮最终显示标题一定是 `下料表`，需要确认命令 caption / resource 绑定。
- 还不能证明 Dialog #427 一定由 `psexcel` 打开。
- 还不能证明运行时输出目录、文件名、覆盖策略或 Excel / Detail 包写出行为。

结论：

```text
下料表 UI 入口已经从“内部命令候选”推进到：

工程图 / 输出 Ribbon 分组 -> 0x8D1C -> psexcel -> sub_140605B20

但 Dialog #427 和运行输出仍需旧图石运行确认。
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

和 `E-IDA-019 / E-IDA-020 / E-IDA-021` 的边界：

```text
E-IDA-019/020/021 确认 psexcel / 0x8D1C / sub_140605B20 / sub_140602F90
这条 Excel / 下料命令链。

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

## 待继续分析

- `sub_1404DE110` 和 `sub_1404DE720` 已完成第二轮 IDA MCP 补证，公共生成链已追到 `sub_1404D10C0 -> sub_140451730 -> sub_1405D5670 -> sub_1405BD0C0 / sub_1405C7260 / sub_1405E49D0`。
- `sub_1405D5670` 已确认 split / spline / trim / min-distance / 写回主规则，但第 4 个 double 参数来源、字段业务名和对象名仍需继续闭合。
- `TODO-046 / E-IDA-029` 已把 `JointRuler / JointDistbet / JointWeldLength` 的旧参数链、`JointWeldLength / 2000.0` 半长公式、`pattern` raw byte `'L'`、`Others / symbolcutIOS` gate 和额外弧线 / `DrawTaoTong` 关系继续补证；但 owning enum / 结构名、旧 UI 触发、旧运行非空样例和旧插件接受度仍需继续闭合。
- `TODO-048 / E-IDA-030` 已把旧图石启动期阻塞链闭合到 `sub_1406BBFC0 -> sub_1406BC3B0 -> sub_14070C760`，并确认 `41 -> 许可已过期`、其他非 0 -> `请检查网线是否接好`、`ChaspBase` 许可对象和 `HASP / SuperDog / NetHASP` 许可栈侧证据；但当前本机究竟卡在“许可过期 / 网络不可达 / 许可服务未就绪 / 许可文件链异常”的哪一种真实环境原因，仍需用户手工确认。
- `rebarz` / `rebarpost` 业务含义未闭合。
- 顶部 Ribbon 按钮和英文命令并非一一对应，需要结合运行界面确认。
- `生成工程图` 顶部按钮的命令表入口仍需继续追。
